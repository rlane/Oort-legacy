// Copyright 2011 Rich Lane
#include "sim/bullet.h"

#include <Box2D/Box2D.h>

#include "sim/ai.h"
#include "sim/team.h"
#include "sim/game.h"
#include "common/log.h"

using glm::vec2;

namespace Oort {

Bullet::Bullet(Game *game, std::shared_ptr<Team> team)
  : Entity(game, team),
    creation_time(game->time) {
	b2CircleShape shape;
	shape.m_radius = 0.1f;
	auto x = body->CreateFixture(&shape, 1.0f);
	b2Filter filter;
	filter.categoryBits = 0x2;
	filter.maskBits = 0x1;
	x->SetFilterData(filter);
}

Bullet::~Bullet() {
}

void Bullet::tick() {
	if (game->time > creation_time + lifetime) {
		dead = true;
	}
}

}
