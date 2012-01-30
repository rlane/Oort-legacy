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

class FramerateCounter {
public:
	int count;
	uint64_t start;
	uint64_t prev;
	uint64_t instant;
	float hz;

	FramerateCounter() {
		count = 0;
		start = prev = microseconds();
		hz = 0;
	}

	bool update() {
		count++;
		auto now = microseconds();
		instant = now - prev;
		prev = now;
		auto elapsed = now - start;
		if (elapsed >= 1000000LL) {
			hz = 1.0e6*count/elapsed;
			count = 0;
			start = now;
			return true;
		}
		return false;
	}
};

class OortInstance : public pp::Instance {
	Game *game;
	pp::Graphics3D gl_context;
	std::unique_ptr<Renderer> renderer;
	int screen_width, screen_height;
	pthread_mutex_t mutex;
	FramerateCounter framerate;
	FramerateCounter tickrate;

	public:
	explicit OortInstance(PP_Instance instance)
		: pp::Instance(instance)
	{
		pthread_mutex_init(&mutex, NULL);
	}

	static void static_swap_callback(void* user_data, int32_t result);

	void schedule_swap() {
		//log("scheduling swap: fn=%p data=%p", static_swap_callback, this);
		pp::CompletionCallback cb(static_swap_callback, this);
		gl_context.SwapBuffers(cb);
		//log("swap scheduled");
	}

	void swap_callback() {
		const float view_radius = 300;
		const glm::vec2 view_center = glm::vec2(0,0);
		//log("rendering");
		{
			pthread_mutex_lock(&mutex);
			renderer->render(view_radius, view_center, 0.0f);
			pthread_mutex_unlock(&mutex);
		}
		//log("rendered");
		schedule_swap();

		if (framerate.update()) {
			log("%0.2f fps", framerate.hz);
			log("%0.2f tps", tickrate.hz);
			renderer->dump_perf();
		}
	}

	void handle_resize(int w, int h) {
		screen_width = w;
		screen_height = h;
		glViewport(0, 0, w, h);
		renderer->reshape(w, h);
	}

	void ticker_func() {
		const uint64_t target = 31250;

		while (true) {
			Timer timer;

			game->tick();

			{
				pthread_mutex_lock(&mutex);
				renderer->tick(*game);
				pthread_mutex_unlock(&mutex);
			}

			tickrate.update();
			auto elapsed = timer.elapsed();
			int remaining = int(target) - int(elapsed);
			usleep(glm::max(remaining, 1000));
		}
	}

	static void *static_ticker_func(void *inst) {
		static_cast<OortInstance*>(inst)->ticker_func();
		return NULL;
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
		game = new Game(scn, ai_factories);

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

		renderer = std::unique_ptr<Renderer>(new Renderer());
		log("renderer created");

		handle_resize(initial_screen_width, initial_screen_height);
		log("screen resized");

		glClearColor(1.0f, 1.0f, 0.5, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		log("cleared");

		pthread_t ticker;
		pthread_create(&ticker, NULL, static_ticker_func, this);
		schedule_swap();

		return true;
	}

	// Called whenever the in-browser window changes size.
	virtual void DidChangeView(const pp::Rect& position, const pp::Rect& clip) {
		log("DidChangeView");
		auto size = position.size();
		gl_context.ResizeBuffers(size.width(), size.height());
		handle_resize(size.width(), size.height());
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
