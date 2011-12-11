// Copyright 2011 Rich Lane
#include "sim/ship_class.h"

#include <iostream>

namespace Oort {

ShipClass *fighter;

void ShipClass::initialize() {
	fighter = new ShipClass();
	auto shape = new b2PolygonShape();
	fighter->shape = shape;
	b2Vec2 vertices[] = { b2Vec2(-0.7, -0.71),
	                      b2Vec2(1, 0),
	                      b2Vec2(-0.7, 0.71) };
	shape->Set(vertices, 3);
	fighter->mass = 10e3;
	b2MassData md;
	fighter->shape->ComputeMass(&md, 1);
	fighter->density = fighter->mass/md.mass;

#if 1
	fighter->shape->ComputeMass(&md, fighter->density);
	std::cout << "resulting mass: " << md.mass << std::endl;
	std::cout << "resulting center of mass: (" << md.center.x << ", " << md.center.y << ")" << std::endl;
#endif
}

}
