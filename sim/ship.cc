// Copyright 2011 Rich Lane
#include "sim/ship.h"

#include <Box2D/Box2D.h>

#include "sim/ai.h"
#include "sim/team.h"
#include "sim/game.h"
#include "sim/bullet.h"
#include "sim/ship_class.h"
#include "common/log.h"

using glm::vec2;

namespace Oort {

static uint32_t next_id = 1;

Ship::Ship(Game *game, const ShipClass *klass, std::shared_ptr<Team> team)
	: Entity(game, team),
	  klass(klass),
	  ai(new AI(this, team->ai)),
	  id(next_id++) { // XXX
	body->CreateFixture(&klass->shape, klass->density);
}

Ship::~Ship() {
}

void Ship::tick() {
	ai->tick();
	update_forces();
}

void Ship::fire(float angle) {
	auto bullet = std::make_shared<Bullet>(game, team, id);
	auto t = body->GetTransform();
	auto v = body->GetLinearVelocity();
	v += 30.0f*b2Vec2(cos(angle), sin(angle));
	bullet->body->SetTransform(t.p, angle);
	bullet->body->SetLinearVelocity(v);
	game->bullets.push_back(bullet);
}

void Ship::acc_main(float acc) {
	main_acc = acc;
}

void Ship::acc_lateral(float acc) {
	lateral_acc = acc;
}

void Ship::acc_angular(float acc) {
	angular_acc = acc;
}

void Ship::update_forces() {
	b2MassData md;
	body->GetMassData(&md);
	float main_thrust = main_acc * md.mass;
	float lateral_thrust = lateral_acc * md.mass;
	float torque = angular_acc * md.I;
	auto t = body->GetTransform();
	auto local_force_vec = b2Vec2(main_thrust, lateral_thrust);
	auto world_force_vec = b2Mul(t.q, local_force_vec);
	body->ApplyForceToCenter(world_force_vec);
	body->ApplyTorque(torque);
}

}
