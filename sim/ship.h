// Copyright 2011 Rich Lane
#ifndef OORT_SIM_SHIP_H_
#define OORT_SIM_SHIP_H_

#include <memory>
#include "sim/physics.h"
#include "sim/team.h"

namespace Oort {

class Ship {
	public:
	Physics physics;
	std::shared_ptr<Team> team;
	std::unique_ptr<AI> ai;

	Ship(std::shared_ptr<Team> team);
	~Ship();

	void tick();

	Ship(const Ship&) = delete;
	Ship& operator=(const Ship&) = delete;
};

}

#endif
