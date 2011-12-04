// Copyright 2011 Rich Lane
#include "sim/ship.h"

#include <Box2D/Box2D.h>

#include "sim/ai.h"
#include "sim/team.h"
#include "sim/game.h"
#include "sim/bullet.h"
#include "common/log.h"

using glm::vec2;

namespace Oort {

static uint32_t next_id = 1;

Ship::Ship(Game *game, std::shared_ptr<Team> team)
	: Entity(game, team),
	  ai(new AI(this, team->ai)),
	  id(next_id++) { // XXX
	b2CircleShape shape;
	shape.m_radius = 1.0f;
	body->CreateFixture(&shape, 1.0f);
}

Ship::~Ship() {
}

void Ship::tick() {
	ai->tick();
}

void Ship::fire() {
	auto bullet = std::make_shared<Bullet>(game, team, id);
	auto t = body->GetTransform();
	auto h = t.q.GetAngle();
	auto v = body->GetLinearVelocity();
	v += 30.0f*b2Vec2(cos(h), sin(h));
	bullet->body->SetTransform(t.p, h);
	bullet->body->SetLinearVelocity(v);
	game->bullets.push_back(bullet);
}

}
