// Copyright 2011 Rich Lane
#ifndef OORT_SIM_BULLET_H_
#define OORT_SIM_BULLET_H_

#include <stdint.h>
#include <memory>
#include "sim/entity.h"

namespace Oort {

class Game;
class GunDef;

class Bullet : public Entity {
	public:
	uint32_t creator_id;
	float creation_time;
	float lifetime;

	Bullet(Game *game, std::shared_ptr<Team> team, uint32_t creator_id, const GunDef &gun);
	~Bullet();

	virtual void tick();
};

}

#endif

