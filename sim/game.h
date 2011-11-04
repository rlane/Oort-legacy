// Copyright 2011 Rich Lane

#ifndef OORT_SIM_GAME_H_
#define OORT_SIM_GAME_H_

#include <list>
#include <memory>
#include <vector>
#include "sim/ship.h"

namespace Oort {

class Scenario;
class AI;

class Game {
	public:

	Game(Scenario &scn, std::vector<std::shared_ptr<AI>> &ais);
	~Game();

	void tick();
	std::shared_ptr<Team> check_victory();

	std::list<std::shared_ptr<Ship>> ships;
};

}

#endif
