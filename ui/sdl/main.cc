// Copyright 2011 Rich Lane
#include <boost/program_options.hpp>
#include "ui/gui.h"
#include "gl/gl.h"
#include <SDL.h>
#include "sim/game.h"
#include "sim/test.h"
#include "sim/ship_class.h"
#include "sim/scenario.h"
#include "sim/builtin_ai.h"

using glm::vec2;
using std::make_shared;
using std::shared_ptr;
using boost::format;
using boost::str;
namespace po = boost::program_options;

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
	  case SDLK_n: return 'n';
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
			if (event.button.button == SDL_BUTTON_WHEELUP ||
			    event.button.button == SDL_BUTTON_WHEELDOWN) {
				gui->handle_scroll(event.button.button == SDL_BUTTON_WHEELUP);
			} else {
				gui->handle_mousebuttondown(event.button.button, event.button.x, event.button.y);
			}
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
	std::shared_ptr<Game> game;
	Test *test = NULL;

	ShipClass::initialize();

	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("test,t", po::value<std::string>(), "test to run")
		("scenario,s", po::value<std::string>(), "scenario")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cerr << desc << std::endl;
		return 0;
	}

	if (vm.count("test") && vm.count("scenario")) {
		fprintf(stderr, "both test or scenario specified\n");
		return 1;
	}

	if (vm.count("test")) {
		std::string test_path = vm["test"].as<std::string>();
		printf("Running test %s\n", test_path.c_str());
		test = Test::load(test_path);
		game = test->get_game();
	} else if (vm.count("scenario")) {
		std::string scn_path = vm["scenario"].as<std::string>();
		printf("Running scenario %s\n", scn_path.c_str());
		Scenario scn = Scenario::load(scn_path);
		std::vector<std::shared_ptr<AIFactory>> ai_factories{ builtin_ai_factory, builtin_ai_factory, builtin_ai_factory };
		game = std::make_shared<Game>(scn, ai_factories);
	} else {
		fprintf(stderr, "no test or scenario specified\n");
		return 1;
	}

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
	gui->start();

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

	gui->stop();

	return 0;
}

}

int main(int argc, char **argv) {
	return Oort::main(argc, argv);
}
