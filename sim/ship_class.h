// Copyright 2011 Rich Lane
#ifndef OORT_SIM_SHIP_CLASS_H_
#define OORT_SIM_SHIP_CLASS_H_

#include <Box2D/Box2D.h>

namespace Oort {

class ShipClass {
public:
	b2Shape *shape;
	float mass;
	float density;

	static void initialize();
};

extern ShipClass *fighter;

}

#endif
