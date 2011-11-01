// Copyright 2011 Rich Lane
#include <boost/foreach.hpp>
#include <memory>

#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

#include "common/log.h"
#include "sim/ship.h"
#include "sim/team.h"
#include "sim/game.h"

using glm::vec2;
using glm::dvec2;
using std::make_shared;

namespace Oort {

int main(int argc, char **argv) {
	dvec2 b(2.0f, 3.0f);
	auto red_team = make_shared<Team>("red", glm::vec3(1, 0, 0));
	auto ship = make_shared<Oort::Ship>(red_team);
	ship->physics.v = dvec2(2.0, 3.0);
	ship->physics.tick(1.0/32);
	log("position: %s\n", glm::to_string(ship->physics.p).c_str());
	auto game = make_shared<Oort::Game>();
	game->ships.push_back(make_shared<Oort::Ship>(red_team));
	BOOST_FOREACH(auto ship, game->ships) {
		ship->physics.v = dvec2(1.0, 1.0);
		ship->physics.tick(1.0/32);
		log("position: %s\n", glm::to_string(ship->physics.p).c_str());
	}
	return 0;
}

}

int main(int argc, char **argv) {
	return Oort::main(argc, argv);
}
