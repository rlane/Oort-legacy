// Copyright 2011 Rich Lane
#include "sim/ship.h"

#include <Box2D/Box2D.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include "glm/gtx/rotate_vector.hpp"

#include "sim/ai.h"
#include "sim/team.h"
#include "sim/game.h"
#include "sim/bullet.h"
#include "sim/ship_class.h"
#include "sim/math_util.h"
#include "common/log.h"

using glm::vec2;

namespace Oort {

static uint32_t next_id = 1;

Ship::Ship(Game *game, const ShipClass &klass, std::shared_ptr<Team> team)
	: Entity(game, team),
	  klass(klass),
	  ai(new AI(this, team->ai)),
	  id(next_id++), // XXX
	  prng(id) { // XXX
	hull = klass.hull;
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

void Ship::fire(float angle) {
	boost::random::normal_distribution<float> v_dist(1000, 10);
	auto bullet = std::make_shared<Bullet>(game, team, id);
	auto p = get_position();
	auto v = get_velocity() + v_dist(prng) * vec2(cos(angle), sin(angle));
	bullet->set_position(p);
	bullet->set_heading(angle);
	bullet->set_velocity(v);
	game->bullets.push_back(bullet);
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
