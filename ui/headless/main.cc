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

using glm::vec2;
using std::make_shared;
namespace po = boost::program_options;

namespace Oort {

int main(int argc, char **argv) {
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("test,t", po::value<std::string>(), "test to run")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cerr << desc << std::endl;
		return 0;
	}

	if (!vm.count("test")) {
		fprintf(stderr, "no test specified\n");
		return 1;
	}

	std::string test_path = vm["test"].as<std::string>();

	ShipClass::initialize();

	printf("Running test %s\n", test_path.c_str());
	auto test = Test::load(test_path);
	auto game = test->get_game();

	b2Timer timer;

	while (true) {
		if (test->finished) {
			printf("Passed test in %d ticks\n", game->ticks);
			break;
		}

		game->tick();
		test->after_tick();
	}

	printf("ms/frame: %0.2f\n", timer.GetMilliseconds()/game->ticks);

	return 0;
}

}

int main(int argc, char **argv) {
	return Oort::main(argc, argv);
}
