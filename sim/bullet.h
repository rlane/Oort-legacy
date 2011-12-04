// Copyright 2011 Rich Lane
#ifndef OORT_SIM_BULLET_H_
#define OORT_SIM_BULLET_H_

#include <memory>
#include "sim/entity.h"

namespace Oort {

class Team;
class Game;

class Bullet : public Entity {
	public:
	std::shared_ptr<Team> team;
	float creation_time;
	float lifetime;

	Bullet(Game *game, std::shared_ptr<Team> team);
	~Bullet();

	virtual void tick();
};

}

#endif

