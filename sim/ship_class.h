// Copyright 2011 Rich Lane
#ifndef OORT_SIM_SHIP_CLASS_H_
#define OORT_SIM_SHIP_CLASS_H_

#include <vector>
#include <Box2D/Box2D.h>
#include "glm/glm.hpp"

namespace Oort {

class ShipClass {
public:
	b2Shape *shape;
	float mass;
	float density;
	std::vector<glm::vec2> vertices;

	static void initialize();
};

extern ShipClass *fighter;

}

#endif
