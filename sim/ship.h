// Copyright 2011 Rich Lane
#ifndef OORT_SIM_SHIP_H_
#define OORT_SIM_SHIP_H_

#include <memory>
#include "sim/entity.h"

namespace Oort {

class Team;
class AI;
class Game;

class Ship : public Entity {
	public:
	std::shared_ptr<Team> team;
	std::unique_ptr<AI> ai;

	Ship(Game *game, std::shared_ptr<Team> team);
	~Ship();

	virtual void tick();
};

}

#endif
