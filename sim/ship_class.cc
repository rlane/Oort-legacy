// Copyright 2011 Rich Lane
#include "sim/ship_class.h"

#include <iostream>
#include <boost/foreach.hpp>

namespace Oort {

ShipClass *fighter;

void ShipClass::initialize() {
	std::vector<glm::vec2> vertices = { glm::vec2(-0.7, -0.71),
	                                    glm::vec2(1, 0),
	                                    glm::vec2(-0.7, 0.71) };
	fighter = new ShipClass("fighter", vertices, 10e3, 450e3);
}

ShipClass::ShipClass(const std::string &name,
                     std::vector<glm::vec2> _vertices,
                     float mass,
										 float hull)
  : name(name),
    vertices(_vertices),
    mass(mass),
    hull(hull) {
	shape.Set((b2Vec2*) &vertices[0], vertices.size());

	// calculate density for desired mass
	b2MassData md;
	shape.ComputeMass(&md, 1);
	density = mass/md.mass;

	// move center of mass to local origin
	BOOST_FOREACH(glm::vec2 &v, vertices) {
		v -= glm::vec2(md.center.x, md.center.y);
	}
	shape.Set((b2Vec2*) &vertices[0], vertices.size());
}

}
