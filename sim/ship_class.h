// Copyright 2011 Rich Lane
#ifndef OORT_SIM_SHIP_CLASS_H_
#define OORT_SIM_SHIP_CLASS_H_

#include <vector>
#include <Box2D/Box2D.h>
#include "glm/glm.hpp"

namespace Oort {

class ShipClass {
public:
	b2PolygonShape shape;
	std::vector<glm::vec2> vertices;
	float mass;
	float density;

	static void initialize();

	ShipClass(std::vector<glm::vec2> vertices, float mass);
};

extern ShipClass *fighter;

}

#endif
