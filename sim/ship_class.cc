// Copyright 2011 Rich Lane
#include "sim/ship_class.h"

#include <iostream>
#include <boost/foreach.hpp>
#include "sim/math_util.h"

namespace Oort {

ShipClass *fighter;

void ShipClass::initialize() {
	std::vector<glm::vec2> vertices = { glm::vec2(-0.7, -0.71),
	                                    glm::vec2(1, 0),
	                                    glm::vec2(-0.7, 0.71) };
	BOOST_FOREACH(glm::vec2 &v, vertices) {
		v *= 10;
	}

	fighter = new ShipClass("fighter", vertices, 10e3, 45e6);
}

ShipClass::ShipClass(const std::string &name,
                     std::vector<glm::vec2> _vertices,
                     float mass,
                     float hull)
  : name(name),
    vertices(_vertices),
    mass(mass),
    hull(hull),
    max_main_acc(100),
    max_lateral_acc(50),
    max_angular_acc(2) {
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

	auto physics_vertices = vertices;
	BOOST_FOREACH(glm::vec2 &v, physics_vertices) {
		v *= (1.0/Oort::SCALE);
	}
	shape.Set((b2Vec2*) &physics_vertices[0], physics_vertices.size());
}

}
