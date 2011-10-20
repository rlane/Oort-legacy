#include <iostream>
#include <memory>
#include <boost/foreach.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <SDL.h>
#include <SDL_opengl.h>

#include "sim/ship.h"
#include "sim/game.h"

using glm::vec2;
using glm::dvec2;
using std::make_shared;

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Surface *screen = SDL_SetVideoMode(1600, 900, 16, SDL_OPENGL | SDL_FULLSCREEN);

	auto a = new vec2(1.0f,2.0f);
	dvec2 b(2.0f,3.0f);
	auto ship = make_shared<Oort::Ship>();
	ship->physics.v = dvec2(2.0, 3.0);
	ship->physics.tick(1.0/32);
	std::cout << "position:" << glm::to_string(ship->physics.p) << std::endl;
	auto game = make_shared<Oort::Game>();
	game->ships.push_back(make_shared<Oort::Ship>());
	BOOST_FOREACH(auto ship, game->ships) {
		ship->physics.v = dvec2(1.0, 1.0);
		ship->physics.tick(1.0/32);
		std::cout << "position:" << glm::to_string(ship->physics.p) << std::endl;
	}
	return 0;
}
