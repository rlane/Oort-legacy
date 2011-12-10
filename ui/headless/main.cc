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
#include "sim/test.h"

using glm::vec2;
using std::make_shared;

namespace Oort {

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s test.so\n", argv[0]);
		return 1;
	}

	printf("Running test %s\n", argv[1]);
	auto test = new Test(std::string(argv[1]));
	auto game = test->game;

	b2Timer timer;

	while (true) {
		test->hook("tick");

		if (!test) {
			auto winner = game->check_victory();
			if (winner != nullptr) {
				printf("Team %s is victorious\n", winner->name.c_str());
			}
		} else {
			if (game->test_finished) {
				printf("Passed test in %d ticks\n", game->ticks);
				break;
			}
		}

		game->tick();
	}

	printf("ms/frame: %0.2f\n", timer.GetMilliseconds()/game->ticks);

	return 0;
}

}

int main(int argc, char **argv) {
	return Oort::main(argc, argv);
}
