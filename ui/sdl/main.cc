// Copyright 2011 Rich Lane
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <GL/glew.h>
#include <SDL.h>
#define NO_SDL_GLEXT
#include <SDL_opengl.h>
#include <boost/timer.hpp>
#include <memory>
#include <vector>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "common/log.h"
#include "common/resources.h"
#include "sim/ship.h"
#include "sim/game.h"
#include "sim/scenario.h"
#include "sim/team.h"
#include "renderer/renderer.h"
#include "renderer/physics_debug_renderer.h"

using glm::vec2;
using std::make_shared;
using std::shared_ptr;

// needs -lrt (real-time lib)
// 1970-01-01 epoch UTC time, 1 mcs resolution (divide by 1M to get time_t)
uint64_t ClockGetTime()
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000000LL + (uint64_t)ts.tv_nsec / 1000LL;
}

namespace Oort {

static enum class State {
	RUNNING,
	FINISHED
} state = State::RUNNING;

static bool running = true;
static bool paused = false;
static bool single_step = false;
static bool render_physics_debug = false;

static void handle_sdl_event(const SDL_Event &event) {
	switch(event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_SPACE:
					paused = !paused;
					break;
				case SDLK_RETURN:
					paused = false;
					single_step = true;
					break;
				case SDLK_g:
					render_physics_debug = !render_physics_debug;
					break;
				default:
					break;
			}
			break;
		case SDL_KEYUP:
			break;
		case SDL_QUIT:
			running = false;
			break;
	}
}

int main(int argc, char **argv) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	if (SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1) != 0) {
		printf("unable to configure vsync\n");
	}

	int w = 1600, h = 900;
	SDL_SetVideoMode(w, h, 16, SDL_OPENGL | SDL_FULLSCREEN);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glViewport(0, 0, 1600, 900);

	AISourceCode ai{"foo.lua", ""};
	Scenario scn;
	std::vector<AISourceCode> ais{ ai, ai, ai };
	auto game = make_shared<Oort::Game>(scn, ais);

	SDL_Event event;

	Renderer renderer(game);
	renderer.set_screen_dimensions(w, h);

	PhysicsDebugRenderer physics_debug_renderer;
	physics_debug_renderer.set_screen_dimensions(w, h);
	physics_debug_renderer.SetFlags(b2Draw::e_shapeBit);
	game->world->SetDebugDraw(&physics_debug_renderer);

	auto prev = ClockGetTime();
	int frame_count = 0;

	while (running) {
		while(SDL_PollEvent(&event)) {
			handle_sdl_event(event);
		}

		if (!paused) {
			if (single_step) {
				single_step = false;
				paused = true;
			}

			game->tick();

			if (state == State::RUNNING) {
				auto winner = game->check_victory();
				if (winner != nullptr) {
					printf("Team %s is victorious\n", winner->name.c_str());
					state = State::FINISHED;
				}
			}
		}

		renderer.render();

		if (render_physics_debug) {
			physics_debug_renderer.begin_render();
			physics_debug_renderer.DrawCircle(b2Vec2(0,0), 10.0f, b2Color(1,1,1));
			game->world->DrawDebugData();
			physics_debug_renderer.end_render();
		}

		SDL_GL_SwapBuffers();

		++frame_count;
		auto now = ClockGetTime();
		auto elapsed = now - prev;
		if (elapsed >= 1000000LL) {
			printf("%0.2f fps\n", 1e6*frame_count/elapsed);
			frame_count = 0;
			prev = now;
		}
	}

	return 0;
}

}

int main(int argc, char **argv) {
	return Oort::main(argc, argv);
}
