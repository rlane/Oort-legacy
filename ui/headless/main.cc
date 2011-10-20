#include <iostream>
#include <memory>
#include <boost/foreach.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "sim/ship.h"
#include "sim/game.h"

using glm::vec2;
using glm::dvec2;
using std::make_shared;

int main(int argc, char **argv)
{
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
