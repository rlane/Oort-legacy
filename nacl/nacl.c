/* Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/pp_var.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/c/ppb_messaging.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_messaging.h"

struct MessageInfo {
  PP_Instance instance;
  struct PP_Var message;
};

static struct PPB_Messaging* ppb_messaging_interface = NULL;
static struct PPB_Var* ppb_var_interface = NULL;
static PP_Module module_id = 0;

void oort_start(void);

static PP_Bool Instance_DidCreate(PP_Instance instance,
                                  uint32_t argc,
                                  const char* argn[],
                                  const char* argv[]) {

	printf("Instance_DidCreate\n");
  oort_start();
  return PP_TRUE;
}

static void Instance_DidDestroy(PP_Instance instance) {
	printf("Instance_DidDestroy\n");
}

static void Instance_DidChangeView(PP_Instance instance,
                                   const struct PP_Rect* position,
                                   const struct PP_Rect* clip) {
	printf("Instance_DidChangeView\n");
}

static void Instance_DidChangeFocus(PP_Instance instance,
                                    PP_Bool has_focus) {
	printf("Instance_DidChangeFocus\n");
}

static PP_Bool Instance_HandleDocumentLoad(PP_Instance instance,
                                           PP_Resource url_loader) {
  return PP_FALSE;
}

void Messaging_HandleMessage(PP_Instance instance, struct PP_Var var_message) {
	printf("Messaging_HandleMessage\n");
}

PP_EXPORT int32_t PPP_InitializeModule(PP_Module a_module_id,
                                       PPB_GetInterface get_browser) {
	printf("PPP_InitializeModule\n");
  g_type_init();
  module_id = a_module_id;
  ppb_messaging_interface = (struct PPB_Messaging*)(get_browser(PPB_MESSAGING_INTERFACE));
  ppb_var_interface = (struct PPB_Var*)(get_browser(PPB_VAR_INTERFACE));

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
  }
  return NULL;
}

PP_EXPORT void PPP_ShutdownModule() {
	printf("PPP_ShutdownModule\n");
}
