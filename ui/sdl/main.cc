// Copyright 2011 Rich Lane
#include <stdio.h>
#include <GL/glew.h>
#include <SDL.h>
#define NO_SDL_GLEXT
#include <SDL_opengl.h>
#include <boost/foreach.hpp>
#include <fstream>
#include <memory>
#include <vector>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

#include "sim/ship.h"
#include "sim/game.h"

#include "gl/check.h"
#include "gl/shader.h"
#include "gl/program.h"

using glm::vec2;
using glm::dvec2;
using std::make_shared;
using std::shared_ptr;

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

static std::string read_file(std::string filename) {
	typedef std::istream_iterator<char> istream_iterator;
	typedef std::ostream_iterator<char> ostream_iterator;
	std::ifstream file;
	file.exceptions(std::ifstream::badbit);
	file.open(filename, std::ios::in|std::ios::binary|std::ios::ate);
	file >> std::noskipws;
	auto size = file.tellg();
	printf("reading %s size %d\n", filename.c_str(), static_cast<int>(size));
	std::string data;
	data.reserve(size);
	file.seekg(0, std::ios::beg);
	std::copy(istream_iterator(file), istream_iterator(),
			      std::back_inserter(data));
	file.close();
	return data;
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

	GL::Program prog(
		make_shared<GL::VertexShader>(read_file("shaders/ship.v.glsl")),
		make_shared<GL::FragmentShader>(read_file("shaders/ship.f.glsl")));

	dvec2 b(2.0f, 3.0f);
	auto ship = make_shared<Oort::Ship>();
	ship->physics.v = dvec2(2.0, 3.0);
	ship->physics.tick(1.0/32);
	auto game = make_shared<Oort::Game>();
	game->ships.push_back(make_shared<Oort::Ship>());

	SDL_Event event;

	glEnable(GL_PROGRAM_POINT_SIZE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glm::mat4 p_matrix = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f);

	while (running) {
		while(SDL_PollEvent(&event)) {
			handle_sdl_event(event);
		}

		GL::check();

		glClear(GL_COLOR_BUFFER_BIT);

		prog.use();
		GL::check();

		BOOST_FOREACH(auto ship, game->ships) {
			ship->physics.v = dvec2(1.0, 1.0);
			ship->physics.tick(1.0/32);

			glm::mat4 mv_matrix;
			glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
			vec2 vertex(ship->physics.p);

			int mv_matrix_loc = glGetUniformLocation(prog.id, "mv_matrix");
			int p_matrix_loc = glGetUniformLocation(prog.id, "p_matrix");
			int color_loc = glGetUniformLocation(prog.id, "color");
			int vertex_loc = glGetAttribLocation(prog.id, "vertex");
			GL::check();

			glUniformMatrix4fv(mv_matrix_loc, 1, false, glm::value_ptr(mv_matrix));
			glUniformMatrix4fv(p_matrix_loc, 1, false, glm::value_ptr(p_matrix));
			glUniform4fv(color_loc, 1, glm::value_ptr(color));
			glVertexAttrib2f(vertex_loc, vertex.x, vertex.y);
			glDrawArrays(GL_POINTS, 0, 1);
			GL::check();
		}

		GL::Program::clear();
		GL::check();

		SDL_GL_SwapBuffers();
		usleep(31250);
	}

	return 0;
}
