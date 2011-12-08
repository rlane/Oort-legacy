// Copyright 2011 Rich Lane
#include "sim/scenario.h"

#include <memory>
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>

#include "glm/glm.hpp"

#include "sim/game.h"
#include "sim/ai.h"

extern "C" {
	Oort::Game *test_init(void);
}

using namespace std;
using namespace Oort;

Game *test_init() {
	AISourceCode ai{"foo.lua", ""};
	Scenario scn;
	std::vector<AISourceCode> ais{ ai, ai, ai };
	cout << "hello world startup" << endl;
	return new Game(scn, ais);
}
