// Copyright 2011 Rich Lane
#include "sim/ship.h"

#include <limits>
#include <Box2D/Box2D.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include "glm/gtx/rotate_vector.hpp"

#include "sim/ai.h"
#include "sim/team.h"
#include "sim/game.h"
#include "sim/bullet.h"
#include "sim/beam.h"
#include "sim/explosion.h"
#include "sim/ship_class.h"
#include "sim/math_util.h"
#include "common/log.h"

using glm::vec2;

namespace Oort {

static uint32_t next_id = 1;

Ship::Ship(Game *game,
		       const ShipClass &klass,
					 std::shared_ptr<Team> team)
	: Entity(game, team),
	  klass(klass),
	  id(next_id++), // XXX
		hull(klass.hull),
	  ai(team->ai_factory->instantiate(*this)),
	  prng(id), // XXX
	  last_fire_times(klass.guns.size(), -std::numeric_limits<float>::infinity()) {
	mass = klass.mass;
	body->CreateFixture(&klass.shape, klass.density);
}

Ship::~Ship() {
}

void Ship::tick() {
	Entity::tick();
	ai->tick();
	update_forces();
}

void Ship::fire_gun(int idx, float angle) {
	if (idx >= (int)klass.guns.size()) {
		return;
	}

	const GunDef &gun = klass.guns[idx];

	float a = angle_diff(get_heading() + gun.angle, angle);
	if (fabsf(a) > gun.coverage/2) {
		return;
	}

	float &lft = last_fire_times[idx];
	if (lft + gun.reload_time > game->time) {
		return;
	} else {
		lft = game->time;
	}

	boost::random::normal_distribution<float> v_dist(gun.velocity, 10);
	boost::random::normal_distribution<float> a_dist(angle, gun.deviation);
	angle = a_dist(prng);
	auto bullet = std::make_shared<Bullet>(game, team, id, gun);
	auto p = get_position() + glm::rotate(gun.origin, glm::degrees(get_heading()));
	auto v = get_velocity() + v_dist(prng) * vec2(cos(angle), sin(angle));
	bullet->set_position(p);
	bullet->set_heading(angle);
	bullet->set_velocity(v);
	game->bullets.push_back(bullet);
}

void Ship::fire_beam(int idx, float angle) {
	if (idx >= (int)klass.beams.size()) {
		return;
	}

	const BeamDef &def = klass.beams[idx];

	float a = angle_diff(get_heading() + def.angle, angle);
	if (fabsf(a) > def.coverage/2) {
		return;
	}

	auto beam = std::make_shared<Beam>(game, team, id, def);
	auto p = get_position() + glm::rotate(def.origin, glm::degrees(get_heading()));
	auto v = get_velocity();
	beam->set_position(p);
	beam->set_heading(angle);
	beam->set_velocity(v);
	b2WeldJointDef joint;
	joint.Initialize(body, beam->get_body(), body->GetPosition());
	game->world->CreateJoint(&joint);
	game->beams.push_back(beam);
}

void Ship::fire_missile(std::weak_ptr<Ship> target) {
	auto ship = std::make_shared<Ship>(game, *missile, team);
	auto origin = vec2(9,3.5);
	auto p = get_position() + glm::rotate(origin, glm::degrees(get_heading()));
	ship->set_position(p);
	ship->set_velocity(get_velocity());
	ship->set_heading(get_heading());
	game->ships.push_back(ship);
}

void Ship::explode() {
	dead = true;
	game->explosions.emplace_back(Explosion{&*team, get_position(), 10e6});
}

void Ship::acc_main(float acc) {
	main_acc = glm::clamp(acc, -klass.max_main_acc, klass.max_main_acc);
}

void Ship::acc_lateral(float acc) {
	lateral_acc = glm::clamp(acc, -klass.max_lateral_acc, klass.max_lateral_acc);
}

void Ship::acc_angular(float acc) {
	angular_acc = glm::clamp(acc, -klass.max_angular_acc, klass.max_angular_acc);
}

void Ship::update_forces() {
	b2MassData md;
	body->GetMassData(&md);
	float main_thrust = main_acc * md.mass;
	float lateral_thrust = lateral_acc * md.mass;
	float torque = angular_acc * md.I;
	auto local_force_vec = vec2(main_thrust, lateral_thrust);
	auto world_force_vec = glm::rotate(local_force_vec, glm::degrees(get_heading()));
	body->ApplyForceToCenter(n2b(world_force_vec));
	body->ApplyTorque(torque);
}

}
