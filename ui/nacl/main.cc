#include "ui/gui.h"
#include <iostream>
#include "gl/gl.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/gles2/gl2ext_ppapi.h"
#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/size.h"
#include "sim/game.h"
#include "sim/scenario.h"
#include "sim/ai.h"
#include "sim/team.h"
#include "sim/ship.h"
#include "sim/ship_class.h"
#include "renderer/renderer.h"
#include "common/log.h"

using namespace Oort;

class OortInstance : public pp::Instance {
	std::shared_ptr<Game> game;
	GUI *gui;
	pp::Graphics3D gl_context;

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
		std::vector<std::shared_ptr<AIFactory>> ai_factories = { CxxAI::factory<CxxAI>(), CxxAI::factory<CxxAI>(), CxxAI::factory<CxxAI>() };
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

		gui = new GUI(game, NULL);
		gui->paused = false;

		gui->handle_resize(initial_screen_width, initial_screen_height);
		log("screen resized");

		glClearColor(1.0f, 1.0f, 0.5, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		log("cleared");

		pthread_t ticker;
		pthread_create(&ticker, NULL, GUI::static_ticker_func, gui);
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
