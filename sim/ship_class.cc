// Copyright 2011 Rich Lane
#include "sim/ship_class.h"

#include <iostream>
#include <boost/foreach.hpp>
#include "sim/math_util.h"
#include "sim/model.h"

namespace Oort {

std::unique_ptr<ShipClass> fighter;

void ShipClass::initialize() {
	ShipClassDef def;
	def.name = "fighter";
	def.mass = 10e3;
	def.hull = 45e6;
	def.max_main_acc = 100;
	def.max_lateral_acc = 50;
	def.max_angular_acc = 2;
	def.scale = 10;
	def.model = Model::load("fighter");
	fighter = std::unique_ptr<ShipClass>(new ShipClass(def));
}

ShipClass::ShipClass(const ShipClassDef &def)
  : ShipClassDef(def)
{
	auto physics_vertices = model->collision_shape.vertices;
	BOOST_FOREACH(glm::vec2 &v, physics_vertices) {
		v *= (scale/Oort::SCALE);
	}
	shape.Set((b2Vec2*) &physics_vertices[0], physics_vertices.size());

	// calculate density for desired mass
	b2MassData md;
	shape.ComputeMass(&md, 1);
	density = mass/md.mass;
}

}
