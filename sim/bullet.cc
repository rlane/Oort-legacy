// Copyright 2011 Rich Lane
#include "sim/bullet.h"

#include <Box2D/Box2D.h>

#include "sim/ai.h"
#include "sim/team.h"
#include "sim/game.h"
#include "sim/math_util.h"
#include "sim/ship_class.h"
#include "sim/ship.h"
#include "common/log.h"

using glm::vec2;

namespace Oort {

Bullet::Bullet(Game *game,
               std::shared_ptr<Team> team,
               uint32_t creator_id,
               const GunDef &def)
  : Weapon(game, team, creator_id, def),
    creation_time(game->time) {
	mass = def.mass;
	b2CircleShape shape;
	shape.m_radius = def.radius/Oort::SCALE;
	body->CreateFixture(&shape, 11320);
	body->SetBullet(true);
}

void Bullet::tick() {
	if (game->time > creation_time + get_def().ttl) {
		dead = true;
	}
}

float Bullet::damage(const Ship &ship) {
	float dv = glm::length(ship.get_velocity() - get_velocity());
	float e = 0.5 * mass * dv*dv;
	//printf("ship %d; bullet %p; damage %g\n", ship.id, this, e);
	return e;
}

bool Bullet::should_collide(const Entity &e) const {
	return e.get_id() != creator_id;
}

}
