// Copyright 2011 Rich Lane
#include "sim/bullet.h"

#include <Box2D/Box2D.h>

#include "sim/ai.h"
#include "sim/team.h"
#include "sim/game.h"
#include "sim/math_util.h"
#include "common/log.h"

using glm::vec2;

namespace Oort {

Bullet::Bullet(Game *game, std::shared_ptr<Team> team, uint32_t creator_id)
  : Entity(game, team),
    creator_id(creator_id),
    creation_time(game->time),
    lifetime(1.0f) {
	mass = 0.3;
	auto radius = 0.01f;
	b2CircleShape shape;
	shape.m_radius = radius/Oort::SCALE;
	body->CreateFixture(&shape, 11320);
	body->SetBullet(true);
}

Bullet::~Bullet() {
}

void Bullet::tick() {
	if (game->time > creation_time + lifetime) {
		dead = true;
	}
}

}
