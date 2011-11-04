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
#include "sim/ai.h"

using glm::vec2;
using glm::dvec2;
using std::make_shared;

namespace Oort {

int main(int argc, char **argv) {
	auto ai = make_shared<AI>("foo.lua", "");
	auto game = make_shared<Oort::Game>();
	Scenario scn;
	scn.setup(*game, { ai, ai, ai });

	while (true) {
		game->tick();
		auto winner = game->check_victory();
		if (winner != nullptr) {
			printf("Team %s is victorious\n", winner->name.c_str());
			break;
		}
	}

	return 0;
}

}

int main(int argc, char **argv) {
	return Oort::main(argc, argv);
}
