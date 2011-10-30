// Copyright 2011 Rich Lane
#include <stdio.h>
#include <GL/glew.h>
#include <SDL.h>
#define NO_SDL_GLEXT
#include <SDL_opengl.h>
#include <boost/foreach.hpp>
#include <memory>
#include <vector>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

#include "common/log.h"
#include "common/resources.h"
#include "sim/ship.h"
#include "sim/game.h"
#include "renderer/renderer.h"

using glm::vec2;
using glm::dvec2;
using std::make_shared;
using std::shared_ptr;

namespace Oort {

static bool running = true;

static void handle_sdl_event(const SDL_Event &event) {
	switch(event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = false;
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

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_SetVideoMode(1600, 900, 16, SDL_OPENGL | SDL_FULLSCREEN);
	glViewport(0, 0, 1600, 900);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	dvec2 b(2.0f, 3.0f);
	auto ship = make_shared<Ship>();
	ship->physics.v = dvec2(2.0, 3.0);
	ship->physics.tick(1.0/32);
	auto game = make_shared<Game>();
	game->ships.push_back(make_shared<Ship>());

	SDL_Event event;

	Renderer renderer(game);

	while (running) {
		while(SDL_PollEvent(&event)) {
			handle_sdl_event(event);
		}

		BOOST_FOREACH(auto ship, game->ships) {
			ship->physics.v = dvec2(1.0, 1.0);
			ship->physics.tick(1.0/32);
		}

		renderer.render();

		SDL_GL_SwapBuffers();
		usleep(31250);
	}

	return 0;
}

}

int main(int argc, char **argv) {
	return Oort::main(argc, argv);
}
