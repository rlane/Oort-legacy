// Copyright 2011 Rich Lane
#include "sim/ship_class.h"

#include <iostream>
#include <boost/foreach.hpp>
#include "sim/math_util.h"
#include "sim/model.h"

using glm::vec2;

namespace Oort {

std::unique_ptr<ShipClass> fighter,
                           ion_cannon_frigate,
                           assault_frigate,
                           missile,
													 target;

void ShipClass::initialize() {
	{
		ShipClassDef def;
		def.name = "fighter";
		def.mass = 10e3;
		def.hull = 4.5e6;
		def.max_main_acc = 100;
		def.max_lateral_acc = 10;
		def.max_angular_acc = 1;
		def.scale = 10;
		def.tail_alpha = 0.1f;
		def.model = Model::load("fighter");
		GunDef gun;
		gun.type = GunType::SLUG;
		gun.mass = 0.03f;
		gun.radius = 0.01f;
		gun.velocity = 3000.0f;
		gun.ttl = 1.0f;
		gun.reload_time = 0.125f;
		gun.deviation = 0.02;
		gun.angle = 0.0;
		gun.coverage = 0.8 * pi;
		gun.origin = vec2(9, 0);
		def.guns.push_back(gun);
		fighter = std::unique_ptr<ShipClass>(new ShipClass(def));
	}

	{
		ShipClassDef def;
		def.name = "ion cannon frigate";
		def.mass = 160e3;
		def.hull = 90e6;
		def.max_main_acc = 20;
		def.max_lateral_acc = 2;
		def.max_angular_acc = 0.5;
		def.scale = 40;
		def.tail_alpha = 0.2f;
		def.model = Model::load("ion_cannon_frigate");
		{
			BeamDef beam;
			beam.damage = 6e6;
			beam.length = 1e3;
			beam.width = 6;
			beam.angle = 0;
			beam.coverage = 0;
			beam.origin = vec2(27.33, 0);
			def.beams.push_back(beam);
		}
		ion_cannon_frigate = std::unique_ptr<ShipClass>(new ShipClass(def));
	}

	{
		ShipClassDef def;
		def.name = "assault frigate";
		def.mass = 160e3;
		def.hull = 135e6;
		def.max_main_acc = 20;
		def.max_lateral_acc = 4;
		def.max_angular_acc = 0.7;
		def.scale = 40;
		def.tail_alpha = 0.2f;
		def.model = Model::load("assault_frigate");
		GunDef gun;
		gun.type = GunType::PLASMA;
		gun.mass = 10.0f;
		gun.radius = 0.01f;
		gun.velocity = 600.0f;
		gun.ttl = 10.0f;
		gun.reload_time = 0.6f;
		gun.deviation = 0.04;
		gun.angle = 0.0;
		gun.coverage = 2 * pi;
		gun.origin = vec2(10, 0);
		def.guns.push_back(gun);
		assault_frigate = std::unique_ptr<ShipClass>(new ShipClass(def));
	}

	{
		ShipClassDef def;
		def.name = "missile";
		def.mass = 200;
		def.hull = 30e3;
		def.max_main_acc = 300;
		def.max_lateral_acc = 150;
		def.max_angular_acc = 3;
		def.scale = 1;
		def.tail_alpha = 0.02f;
		def.model = Model::load("missile");
		missile = std::unique_ptr<ShipClass>(new ShipClass(def));
	}

	{
		ShipClassDef def;
		def.name = "target";
		def.mass = 10e3;
		def.hull = 4.5e6;
		def.max_main_acc = 6400;
		def.max_lateral_acc = 500;
		def.max_angular_acc = M_PI;
		def.scale = 10;
		def.tail_alpha = 0.1f;
		def.model = Model::load("fighter");
		target = std::unique_ptr<ShipClass>(new ShipClass(def));
	}
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

#if 0
	BOOST_FOREACH(auto &gun, guns) {
		printf("class %s gun dps %g range %g\n", name.c_str(), gun.mass * gun.velocity * gun.velocity / gun.reload_time, gun.velocity * gun.ttl);
	}
#endif
}

}
