// Copyright 2011 Rich Lane
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <GL/glew.h>
#include <SDL.h>
#define NO_SDL_GLEXT
#include <SDL_opengl.h>
#include <boost/foreach.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
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
#include "renderer/renderer.h"

using glm::vec2;
using glm::dvec2;
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

static bool running = true;
static bool paused = false;

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

	auto game = make_shared<Game>();
	boost::random::mt19937 prng(42);
	boost::random::normal_distribution<> p_dist(0.0, 1.0);
	boost::random::normal_distribution<> v_dist(0.0, 5.0);

	std::vector<shared_ptr<Team>> teams = {
		make_shared<Team>("red", glm::vec3(1, 0, 0)),
		make_shared<Team>("green", glm::vec3(0, 1, 0)),
		make_shared<Team>("blue", glm::vec3(0, 0, 1)),
	};

	for (auto i = 0; i < 100; i++) {
		auto ship = make_shared<Ship>(teams[i%teams.size()]);
		ship->physics.p = dvec2(p_dist(prng), p_dist(prng));
		ship->physics.v = dvec2(v_dist(prng), v_dist(prng));
		ship->physics.h = 0.0;
		game->ships.push_back(ship);
	}

	SDL_Event event;

	Renderer renderer(game);
	renderer.set_screen_dimensions(w, h);
	auto prev = ClockGetTime();
	int frame_count = 0;

	while (running) {
		while(SDL_PollEvent(&event)) {
			handle_sdl_event(event);
		}

		if (!paused) {
			BOOST_FOREACH(auto ship, game->ships) {
				ship->tick();
				ship->physics.tick(1.0/32);
			}
		}

		renderer.render();

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
