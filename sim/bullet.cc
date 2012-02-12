// Copyright 2011 Rich Lane
#include "sim/bullet.h"

#include <boost/foreach.hpp>
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
               const GunDef &def,
                vec2 p, vec2 v)
	: team(team),
	  def(def),
	  creation_time(game->time),
	  creator_id(creator_id),
	  initial_position(p),
	  velocity(v),
	  dead(false) {
}

float Bullet::damage(const Ship &ship) const {
	float dv = glm::length(ship.get_velocity() - velocity);
	float e = 0.5 * def.mass * dv*dv;
	//printf("ship %d; bullet %p; damage %g\n", ship.id, this, e);
	return e;
}

/*
bool Bullet::should_collide(const Entity &e) const {
	return e.get_id() != creator_id;
}
*/

void Bullet::tick_all(Game &game) {
	BOOST_FOREACH(auto &b, game.bullets) {
		if (game.time > b->creation_time + b->def.ttl) {
			b->dead = true;
		} else {
			// XXX collision detection
		}
	}
}

}
