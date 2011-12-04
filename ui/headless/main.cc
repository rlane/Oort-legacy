// Copyright 2011 Rich Lane
#include <boost/foreach.hpp>
#include <memory>

#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <Box2D/Common/b2Timer.h>

#include "common/log.h"
#include "sim/ship.h"
#include "sim/team.h"
#include "sim/game.h"
#include "sim/scenario.h"
#include "sim/ai.h"

using glm::vec2;
using std::make_shared;

namespace Oort {

int main(int argc, char **argv) {
	AISourceCode ai{"foo.lua", ""};
	Scenario scn;
	std::vector<AISourceCode> ais{ ai, ai, ai };
	auto game = make_shared<Oort::Game>(scn, ais);

	b2Timer timer;

	while (true) {
		game->tick();
		auto winner = game->check_victory();
		if (winner != nullptr) {
			printf("Team %s is victorious\n", winner->name.c_str());
			break;
		}
	}

	printf("ms/frame: %0.2f\n", timer.GetMilliseconds()/game->ticks);

	return 0;
}

}

int main(int argc, char **argv) {
	return Oort::main(argc, argv);
}
