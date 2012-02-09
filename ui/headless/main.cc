// Copyright 2011 Rich Lane
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <memory>

#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

#include "common/log.h"
#include "sim/ship.h"
#include "sim/ship_class.h"
#include "sim/team.h"
#include "sim/game.h"
#include "sim/scenario.h"
#include "sim/ai.h"
#include "sim/test.h"
#include "sim/builtin_ai.h"

using glm::vec2;
using std::make_shared;
namespace po = boost::program_options;

namespace Oort {

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

	b2Timer timer;

	while (true) {
		if (test) {
			if (test->finished) {
				printf("Passed test in %d ticks\n", game->ticks);
				break;
			}
		} else {
			Team *winner = NULL;
			if (game->check_victory(winner)) {
				printf("Team %s won in %d ticks\n", winner->name.c_str(), game->ticks);
				break;
			}
		}

		game->tick();

		if (test) {
			test->after_tick();
		}
	}

	printf("ms/frame: %0.2f\n", timer.GetMilliseconds()/game->ticks);

	return 0;
}

}

int main(int argc, char **argv) {
	return Oort::main(argc, argv);
}
