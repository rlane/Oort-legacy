// Copyright 2011 Rich Lane
#ifndef OORT_SIM_SHIP_H_
#define OORT_SIM_SHIP_H_

#include "sim/physics.h"

namespace Oort {

class Ship {
	public:
	Physics physics;

	Ship();
	~Ship();

	Ship(const Ship&) = delete;
	Ship& operator=(const Ship&) = delete;
};

}

#endif
