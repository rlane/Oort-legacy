// Copyright 2011 Rich Lane
#include "sim/bullet.h"

#include <Box2D/Box2D.h>

#include "sim/ai.h"
#include "sim/team.h"
#include "sim/game.h"
#include "sim/math_util.h"
#include "sim/ship_class.h"
#include "common/log.h"

using glm::vec2;

namespace Oort {

Bullet::Bullet(Game *game,
               std::shared_ptr<Team> team,
               uint32_t creator_id,
               const GunDef &gun)
  : Entity(game, team),
    creator_id(creator_id),
    creation_time(game->time),
    lifetime(gun.ttl) {
	mass = gun.mass;
	b2CircleShape shape;
	shape.m_radius = gun.radius/Oort::SCALE;
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

bool Bullet::is_weapon() {
	return true;
}

}
