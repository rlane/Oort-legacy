// Copyright 2011 Rich Lane
#include "sim/ship_class.h"

#include <iostream>
#include <boost/foreach.hpp>

namespace Oort {

ShipClass *fighter;

void ShipClass::initialize() {
	fighter = new ShipClass();
	fighter->vertices = { glm::vec2(-0.7, -0.71),
	                      glm::vec2(1, 0),
	                      glm::vec2(-0.7, 0.71) };
	fighter->mass = 10e3;

	auto shape = new b2PolygonShape();
	shape->Set((b2Vec2*) &fighter->vertices[0], fighter->vertices.size());
	fighter->shape = shape;

	// calculate density for desired mass
	b2MassData md;
	fighter->shape->ComputeMass(&md, 1);
	fighter->density = fighter->mass/md.mass;

	// move center of mass to local origin
	BOOST_FOREACH(glm::vec2 &v, fighter->vertices) {
		v -= glm::vec2(md.center.x, md.center.y);
	}
	shape->Set((b2Vec2*) &fighter->vertices[0], fighter->vertices.size());

#if 1
	fighter->shape->ComputeMass(&md, fighter->density);
	std::cout << "resulting mass: " << md.mass << std::endl;
	std::cout << "resulting center of mass: (" << md.center.x << ", " << md.center.y << ")" << std::endl;
#endif
}

}
