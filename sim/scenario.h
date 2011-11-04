// Copyright 2011 Rich Lane
#ifndef OORT_SIM_SCENARIO_H_
#define OORT_SIM_SCENARIO_H_

#include <vector>
#include "sim/game.h"

namespace Oort {

class Scenario {
public:

  Scenario();

	void setup(Game &game, std::vector<std::shared_ptr<AI>> ais);
};

}

#endif
