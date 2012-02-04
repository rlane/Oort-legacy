// Copyright 2011 Rich Lane
#include "ui/gui.h"
#include "gl/gl.h"
#include <SDL.h>
#include "sim/game.h"
#include "sim/test.h"
#include "sim/ship_class.h"

using glm::vec2;
using std::make_shared;
using std::shared_ptr;
using boost::format;
using boost::str;

namespace Oort {

static const int initial_screen_width = 800,
						     initial_screen_height = 600;

static GUI *gui;

static uint32_t convert_keycode(uint32_t sym) {
	switch (sym) {
		case SDLK_ESCAPE: return 27;
		case SDLK_SPACE: return ' ';
		case SDLK_RETURN: return '\n';
	  case SDLK_g: return 'g';
	  case SDLK_w: return 'w';
		case SDLK_s: return 's';
	  case SDLK_a: return 'a';
	  case SDLK_d: return 'd';
	  case SDLK_z: return 'z';
	  case SDLK_x: return 'x';
	  case SDLK_b: return 'b';
		default: return '\0';
	}
}

static void handle_sdl_event(const SDL_Event &event) {
	switch(event.type) {
		case SDL_KEYDOWN:
			gui->handle_keydown(convert_keycode(event.key.keysym.sym));
			break;
		case SDL_KEYUP:
			gui->handle_keyup(convert_keycode(event.key.keysym.sym));
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
		int x, y;
		SDL_GetMouseState(&x, &y);
		gui->update_mouse_position(x, y);

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
