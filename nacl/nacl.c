/* Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "oort.h"
#include "oort_renderer.h"

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/pp_var.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/c/ppb_messaging.h"
#include "ppapi/c/ppb_core.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/ppb_input_event.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_messaging.h"
#include "ppapi/c/ppp_input_event.h"
#include "ppapi/c/pp_completion_callback.h"
#include "ppapi/c/ppb_graphics_3d.h"
#include "ppapi/gles2/gl2ext_ppapi.h"
#include "GLES2/gl2.h"

struct MessageInfo {
  PP_Instance instance;
  struct PP_Var message;
};

static struct PPB_Messaging* messaging_interface = NULL;
static struct PPB_Var* var_interface = NULL;
static struct PPB_Graphics3D* graphics3d_interface = NULL;
static struct PPB_Instance* instance_interface = NULL;
static struct PPB_Core* core_interface = NULL;
static struct PPB_InputEvent* input_interface = NULL;
static struct PPB_KeyboardInputEvent* keyboard_interface = NULL;
static struct PPB_MouseInputEvent* mouse_interface = NULL;
static struct PPB_WheelInputEvent* wheel_interface = NULL;
static PP_Module module_id = 0;
static PP_Resource context;

void oort_init(void);
void oort_start(void);
void oort_tick(void);
void oort_handle_message(char *msg);
void oort_reshape(int width, int height);
PP_Bool oort_handle_key(uint32_t keycode);
void oort_handle_mouse_move(int x, int y);
void oort_handle_mouse_click(int x, int y, int button);
void oort_handle_mouse_wheel(float delta);

/**
 * Returns a mutable C string contained in the @a var or NULL if @a var is not
 * string.  This makes a copy of the string in the @a var and adds a NULL
 * terminator.  Note that VarToUtf8() does not guarantee the NULL terminator on
 * the returned string.  See the comments for VarToUtf8() in ppapi/c/ppb_var.h
 * for more info.  The caller is responsible for freeing the returned memory.
 * @param[in] var PP_Var containing string.
 * @return a mutable C string representation of @a var.
 * @note The caller is responsible for freeing the returned string.
 */
static char* AllocateCStrFromVar(struct PP_Var var) {
  uint32_t len = 0;
  if (var_interface != NULL) {
    const char* var_c_str = var_interface->VarToUtf8(var, &len);
    if (len > 0) {
      char* c_str = (char*)malloc(len + 1);
      memcpy(c_str, var_c_str, len);
      c_str[len] = '\0';
      return c_str;
    }
  }
  return NULL;
}

/**
 * Creates a new string PP_Var from C string. The resulting object will be a
 * refcounted string object. It will be AddRef()ed for the caller. When the
 * caller is done with it, it should be Release()d.
 * @param[in] str C string to be converted to PP_Var
 * @return PP_Var containing string.
 */
/* TODO(sdk_user): 3. Uncomment this when you need it.  It is commented out so
 * that the compiler doesn't complain about unused functions.
 */
#if 0
static struct PP_Var AllocateVarFromCStr(const char* str) {
  if (var_interface != NULL)
    return var_interface->VarFromUtf8(module_id, str, strlen(str));
  return PP_MakeUndefined();
}
#endif

static PP_Bool Instance_DidCreate(PP_Instance instance,
                                  uint32_t argc,
                                  const char* argn[],
                                  const char* argv[]) {

	int ret;
	printf("Instance_DidCreate\n");
  oort_init();

  int32_t attribs[] = {PP_GRAPHICS3DATTRIB_WIDTH, 800,
                       PP_GRAPHICS3DATTRIB_HEIGHT, 600,
                       PP_GRAPHICS3DATTRIB_NONE};
  context = graphics3d_interface->Create(instance, 0, attribs);
  if (context == 0) {
    printf("failed to create graphics3d context\n");
    return PP_FALSE;
  }

  glSetCurrentContextPPAPI(context);

  if (!instance_interface->BindGraphics(instance, context)) {
    printf("failed to bind graphics3d context\n");
    return PP_FALSE;
  }

	ret = input_interface->RequestFilteringInputEvents(instance, PP_INPUTEVENT_CLASS_KEYBOARD|PP_INPUTEVENT_CLASS_WHEEL);
	if (ret != PP_OK) {
		printf("failed to request input events\n");
		return PP_FALSE;
	}

	ret = input_interface->RequestInputEvents(instance, PP_INPUTEVENT_CLASS_MOUSE);
	if (ret != PP_OK) {
		printf("failed to request input events\n");
		return PP_FALSE;
	}

  return PP_TRUE;
}

static void Instance_DidDestroy(PP_Instance instance) {
	printf("Instance_DidDestroy\n");
}

static void Instance_DidChangeView(PP_Instance instance,
                                   const struct PP_Rect* position,
                                   const struct PP_Rect* clip) {
	printf("Instance_DidChangeView\n");
	graphics3d_interface->ResizeBuffers(context, position->size.width, position->size.height);
	oort_reshape(position->size.width, position->size.height);
}

static void Instance_DidChangeFocus(PP_Instance instance,
                                    PP_Bool has_focus) {
	printf("Instance_DidChangeFocus: %d\n", has_focus);
}

static PP_Bool Instance_HandleDocumentLoad(PP_Instance instance,
                                           PP_Resource url_loader) {
  return PP_FALSE;
}

static void timersub(struct timeval *a, struct timeval *b, struct timeval *res) {
	const int million = 1000000;
	res->tv_sec = a->tv_sec - b->tv_sec;
	res->tv_usec = a->tv_usec - b->tv_usec;
	if (res->tv_usec < 0) {
		res->tv_usec += million;
		res->tv_sec -= 1;
	} else if (res->tv_usec > million) {
		res->tv_usec -= million;
		res->tv_sec += 1;
	}
}

void tick_callback(void* user_data, int32_t result);

void swap_callback(void* user_data, int32_t result)
{
	struct PP_CompletionCallback cb = { tick_callback, NULL, PP_COMPLETIONCALLBACK_FLAG_NONE };
	static struct timeval last_tv;
	static struct timeval frame_tv = { 0, 31250 };
	struct timeval cur_tv, diff_tv, remaining_tv;
	gettimeofday(&cur_tv, NULL);
	timersub(&cur_tv, &last_tv, &diff_tv);
	timersub(&frame_tv, &diff_tv, &remaining_tv);
	int remaining_ms;
	if (remaining_tv.tv_sec < 0 || remaining_tv.tv_usec < 0) {
		remaining_ms = 0;
	} else {
		remaining_ms = remaining_tv.tv_usec/1000;
	}
	last_tv = cur_tv;
	core_interface->CallOnMainThread(remaining_ms, cb, 0);
}

void tick_callback(void* user_data, int32_t result)
{
	oort_tick();
  struct PP_CompletionCallback callback = { swap_callback, NULL, PP_COMPLETIONCALLBACK_FLAG_NONE };
  int32_t ret = graphics3d_interface->SwapBuffers(context, callback);
  if (ret != PP_OK && ret != PP_OK_COMPLETIONPENDING) {
    printf("SwapBuffers failed with code %d\n", ret);
  }
}

void Messaging_HandleMessage(PP_Instance instance, struct PP_Var var_message) {
	printf("Messaging_HandleMessage\n");
	char *msg = AllocateCStrFromVar(var_message);
	oort_handle_message(msg);
	if (!strcmp(msg, "start")) {
		tick_callback(NULL, 0);
	}
	free(msg);
}


PP_Bool InputEvent_HandleInputEvent(PP_Instance instance, PP_Resource input_event)
{
	PP_InputEvent_Type type = input_interface->GetType(input_event);
	//printf("input event: type %d\n", type);
	if (type == PP_INPUTEVENT_TYPE_KEYDOWN) {
		uint32_t keycode = keyboard_interface->GetKeyCode(input_event);
		return oort_handle_key(keycode);
	} else if (type == PP_INPUTEVENT_TYPE_MOUSEDOWN) {
		struct PP_Point pos = mouse_interface->GetPosition(input_event);
		PP_InputEvent_MouseButton button = mouse_interface->GetButton(input_event);
		oort_handle_mouse_click(pos.x, pos.y, button);
		return TRUE;
	} else if (type == PP_INPUTEVENT_TYPE_WHEEL) {
		struct PP_Point pos = mouse_interface->GetPosition(input_event);
		struct PP_FloatPoint delta = wheel_interface->GetDelta(input_event);
		oort_handle_mouse_wheel(delta.y);
		return TRUE;
	} else if (type == PP_INPUTEVENT_TYPE_MOUSEMOVE) {
		struct PP_Point pos = mouse_interface->GetPosition(input_event);
		oort_handle_mouse_move(pos.x, pos.y);
		return TRUE;
	} else {
		return FALSE;
	}
}

PP_EXPORT int32_t PPP_InitializeModule(PP_Module a_module_id,
                                       PPB_GetInterface get_browser) {
	printf("PPP_InitializeModule\n");
  g_type_init();
  module_id = a_module_id;
  messaging_interface = (struct PPB_Messaging*)(get_browser(PPB_MESSAGING_INTERFACE));
  var_interface = (struct PPB_Var*)(get_browser(PPB_VAR_INTERFACE));
  graphics3d_interface = (struct PPB_Graphics3D*)(get_browser(PPB_GRAPHICS_3D_INTERFACE));
  instance_interface = (struct PPB_Instance*)(get_browser(PPB_INSTANCE_INTERFACE));
  core_interface = (struct PPB_Core*)(get_browser(PPB_CORE_INTERFACE));
  input_interface = (struct PPB_InputEvent*)(get_browser(PPB_INPUT_EVENT_INTERFACE));
  keyboard_interface = (struct PPB_KeyboardInputEvent*)(get_browser(PPB_KEYBOARD_INPUT_EVENT_INTERFACE));
  mouse_interface = (struct PPB_MouseInputEvent*)(get_browser(PPB_MOUSE_INPUT_EVENT_INTERFACE));
  wheel_interface = (struct PPB_WheelInputEvent*)(get_browser(PPB_WHEEL_INPUT_EVENT_INTERFACE));

  if (!glInitializePPAPI(get_browser)) {
    printf("glInitializePPAPI failed\n");
    return PP_ERROR_FAILED;
  }

  return PP_OK;
}

PP_EXPORT const void* PPP_GetInterface(const char* interface_name) {
	printf("PPP_GetInterface(%s)\n", interface_name);
  if (strcmp(interface_name, PPP_INSTANCE_INTERFACE) == 0) {
    static struct PPP_Instance instance_interface = {
      &Instance_DidCreate,
      &Instance_DidDestroy,
      &Instance_DidChangeView,
      &Instance_DidChangeFocus,
      &Instance_HandleDocumentLoad,
    };
    return &instance_interface;
  } else if (strcmp(interface_name, PPP_MESSAGING_INTERFACE) == 0) {
    static struct PPP_Messaging messaging_interface = {
      &Messaging_HandleMessage
    };
    return &messaging_interface;
  } else if (strcmp(interface_name, PPP_INPUT_EVENT_INTERFACE) == 0) {
    static struct PPP_InputEvent input_interface = {
      &InputEvent_HandleInputEvent
    };
    return &input_interface;
	}
  return NULL;
}

PP_EXPORT void PPP_ShutdownModule() {
	printf("PPP_ShutdownModule\n");
}
