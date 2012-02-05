#include "ui/gui.h"
#include <iostream>
#include <unordered_set>
#include "gl/gl.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/gles2/gl2ext_ppapi.h"
#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/size.h"
#include "ppapi/cpp/input_event.h"
#include "sim/game.h"
#include "sim/scenario.h"
#include "sim/ai.h"
#include "sim/team.h"
#include "sim/ship.h"
#include "sim/ship_class.h"
#include "renderer/renderer.h"
#include "common/log.h"

using namespace Oort;

namespace Oort {
	extern CxxAIFactory<CxxAI> default_ai_factory;

	void null_ai_deleter(CxxAIFactory<CxxAI> *x) {}
}

class OortInstance : public pp::Instance {
	std::shared_ptr<Game> game;
	GUI *gui;
	pp::Graphics3D gl_context;
	std::unordered_set<uint32_t> keys_down;

	public:
	explicit OortInstance(PP_Instance instance)
		: pp::Instance(instance)
	{
	}

	static void static_swap_callback(void* user_data, int32_t result);

	void schedule_swap() {
		//log("scheduling swap: fn=%p data=%p", static_swap_callback, this);
		pp::CompletionCallback cb(static_swap_callback, this);
		gl_context.SwapBuffers(cb);
		//log("swap scheduled");
	}

	void swap_callback() {
		gui->render();
		schedule_swap();
	}

	// The dtor makes the 3D context current before deleting the cube view, then
	// destroys the 3D context both in the module and in the browser.
	virtual ~OortInstance() {
		std::cout << "instance destroy" << std::endl;
	}

	// Called by the browser when the NaCl module is loaded and all ready to go.
	virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]) {
		std::cout << "instance init" << std::endl;

		log("initializing ship classes");
		ShipClass::initialize();

		log("creating game");
		Scenario scn = Scenario::load("test/furball.json");
		auto ai_factory = std::shared_ptr<CxxAIFactory<CxxAI>>(&default_ai_factory, null_ai_deleter);
		std::vector<std::shared_ptr<AIFactory>> ai_factories = { ai_factory, ai_factory, ai_factory };
		game = std::make_shared<Game>(scn, ai_factories);

		log("game initialized");

		const int initial_screen_width = 800,
		          initial_screen_height = 600;

		int32_t attribs[] = {
			PP_GRAPHICS3DATTRIB_WIDTH, initial_screen_width,
			PP_GRAPHICS3DATTRIB_HEIGHT, initial_screen_height,
			PP_GRAPHICS3DATTRIB_NONE
		};
		gl_context = pp::Graphics3D(this, pp::Graphics3D(), attribs);
		if (gl_context.is_null()) {
			glSetCurrentContextPPAPI(0);
			return false;
		}
		this->BindGraphics(gl_context);
		glSetCurrentContextPPAPI(gl_context.pp_resource());
		log("graphics bound");

		int ret = RequestFilteringInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD|PP_INPUTEVENT_CLASS_WHEEL);
		if (ret != PP_OK) {
			printf("failed to request input events\n");
			return PP_FALSE;
		}

		ret = RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE);
		if (ret != PP_OK) {
			printf("failed to request input events\n");
			return PP_FALSE;
		}

		gui = new GUI(game, NULL);

		gui->handle_resize(initial_screen_width, initial_screen_height);
		log("screen resized");

		glClearColor(1.0f, 1.0f, 0.5, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		log("cleared");

		gui->start();
		schedule_swap();

		return true;
	}

	// Called whenever the in-browser window changes size.
	virtual void DidChangeView(const pp::Rect& position, const pp::Rect& clip) {
		log("DidChangeView");
		auto size = position.size();
		gl_context.ResizeBuffers(size.width(), size.height());
		gui->handle_resize(size.width(), size.height());
	}

	// Called by the browser to handle the postMessage() call in Javascript.
	virtual void HandleMessage(const pp::Var& message) {
		log("HandleMessage");
	}

	uint32_t convert_key(uint32_t keycode) {
		switch (keycode) {
		case 32: return ' ';
		case 13: return '\n';
		case 71: return 'g';
		case 87: return 'w';
		case 83: return 's';
		case 65: return 'a';
		case 68: return 'd';
		case 90: return 'z';
		case 88: return 'x';
		case 66: return 'b';
		}
		log("unexpected keycode %d", keycode);
		return 0;
	}

	virtual bool HandleInputEvent(const pp::InputEvent &event) {
		PP_InputEvent_Type type = event.GetType();
		if (type == PP_INPUTEVENT_TYPE_KEYDOWN) {
			auto key_event = static_cast<pp::KeyboardInputEvent>(event);
			auto keycode = key_event.GetKeyCode();
			if (keys_down.count(keycode) == 0) {
				keys_down.insert(keycode);
				gui->handle_keydown(convert_key(keycode));
			}
			return true;
		} else if (type == PP_INPUTEVENT_TYPE_KEYUP) {
			auto key_event = static_cast<pp::KeyboardInputEvent>(event);
			auto keycode = key_event.GetKeyCode();
			keys_down.erase(keycode);
			gui->handle_keyup(convert_key(keycode));
			return true;
		} else if (type == PP_INPUTEVENT_TYPE_MOUSEDOWN) {
			auto mouse_event = static_cast<pp::MouseInputEvent>(event);
			auto pos = mouse_event.GetPosition();
			auto button = mouse_event.GetButton();
			int real_button = -1;
			if (button == PP_INPUTEVENT_MOUSEBUTTON_LEFT) {
				real_button = 1;
			}
			gui->handle_mousebuttondown(real_button, pos.x(), pos.y());
			return true;
		} else if (type == PP_INPUTEVENT_TYPE_WHEEL) {
			// TODO smooth scrolling
			auto wheel_event = static_cast<pp::WheelInputEvent>(event);
			gui->handle_scroll(wheel_event.GetDelta().y() > 0);
			return true;
		} else if (type == PP_INPUTEVENT_TYPE_MOUSEMOVE) {
			auto mouse_event = static_cast<pp::MouseInputEvent>(event);
			auto pos = mouse_event.GetPosition();
			gui->update_mouse_position(pos.x(), pos.y());
			return true;
		} else {
			return false;
		}
	}
};

void OortInstance::static_swap_callback(void* user_data, int32_t result)
{
	OortInstance *instance = (OortInstance*)user_data;
	instance->swap_callback();
}

class OortModule : public pp::Module {
public:
	OortModule() : pp::Module() {}

	virtual ~OortModule() {
		glTerminatePPAPI();
	}

	virtual bool Init() {
		log("OortModule::Init");
		return glInitializePPAPI(get_browser_interface()) == GL_TRUE;
	}

	virtual pp::Instance* CreateInstance(PP_Instance instance) {
		log("OortModule::CreateInstance");
		return new OortInstance(instance);
	}
};

namespace pp {
	Module* CreateModule() {
		log("pp::CreateModule");
		return new OortModule();
	}
}
