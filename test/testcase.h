// Copyright 2011 Rich Lane
#include <memory>
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/foreach.hpp>

#include "glm/glm.hpp"

#include "sim/game.h"
#include "sim/ai.h"
#include "sim/scenario.h"
#include "sim/ship.h"
#include "sim/math_util.h"

extern "C" {
	Oort::Game *test_init(void);
	void test_tick(Oort::Game*);
}

using namespace std;
using namespace Oort;
using namespace glm;
