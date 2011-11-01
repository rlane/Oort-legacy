// Copyright 2011 Rich Lane
#include <boost/foreach.hpp>
#include <memory>

#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

#include "common/log.h"
#include "sim/ship.h"
#include "sim/team.h"
#include "sim/game.h"
#include "sim/scenario.h"

using glm::vec2;
using glm::dvec2;
using std::make_shared;

namespace Oort {

int main(int argc, char **argv) {
	auto game = make_shared<Oort::Game>();
	Scenario scn;
	scn.setup(*game);
	BOOST_FOREACH(auto ship, game->ships) {
		ship->tick();
		ship->physics.tick(1.0/32);
	}
	return 0;
}

}

int main(int argc, char **argv) {
	return Oort::main(argc, argv);
}
