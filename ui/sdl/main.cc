// Copyright 2011 Rich Lane
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include "gl/gl.h"
#include <SDL.h>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <boost/format.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "sim/math_util.h"
#include "common/log.h"
#include "common/resources.h"
#include "sim/ship.h"
#include "sim/ship_class.h"
#include "sim/game.h"
#include "sim/scenario.h"
#include "sim/team.h"
#include "renderer/renderer.h"
#include "renderer/physics_debug_renderer.h"
#include "sim/test.h"
#include "ui/gui.h"

using glm::vec2;
using std::make_shared;
using std::shared_ptr;
using boost::format;
using boost::str;

namespace Oort {

static const int initial_screen_width = 800,
						     initial_screen_height = 600;

static GUI *gui;

static void handle_sdl_event(const SDL_Event &event) {
	switch(event.type) {
		case SDL_KEYDOWN:
			gui->handle_keydown(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			gui->handle_keyup(event.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			gui->handle_mousebuttondown(event.button.button, event.button.x, event.button.y);
			break;
		case SDL_VIDEORESIZE:
			SDL_SetVideoMode(event.resize.w, event.resize.h, 32, SDL_OPENGL | SDL_RESIZABLE);
			gui->handle_resize(event.resize.w, event.resize.h);
			break;
		case SDL_QUIT:
			gui->running = false;
			break;
	}
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s test.so\n", argv[0]);
		return 1;
	}

	ShipClass::initialize();

	printf("Running test %s\n", argv[1]);
	auto test = Test::load(std::string(argv[1]));
	auto game = test->get_game();

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1) != 0) {
		printf("unable to set multisample buffers\n");
	}

	if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4) != 0) {
		printf("unable to set multisample samples\n");
	}

	glEnable(GL_MULTISAMPLE);

	if (SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1) != 0) {
		printf("unable to configure vsync\n");
	}

	SDL_SetVideoMode(initial_screen_width, initial_screen_height, 32, SDL_OPENGL | SDL_RESIZABLE);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	gui = new GUI(game, test);

	gui->handle_resize(initial_screen_width, initial_screen_height);

	boost::thread ticker(GUI::static_ticker_func, gui);

	while (gui->running) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			handle_sdl_event(event);
		}

		gui->render();

		SDL_GL_SwapBuffers();
		glFinish();
	}

	ticker.join();

	return 0;
}

}

int main(int argc, char **argv) {
	return Oort::main(argc, argv);
}
