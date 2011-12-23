// Copyright 2011 Rich Lane
#ifndef OORT_SIM_BULLET_H_
#define OORT_SIM_BULLET_H_

#include "sim/weapon.h"

namespace Oort {

class Game;

struct GunDef : public WeaponDef {
	float mass;         // kg
	float radius;       // meters
	float velocity;     // meters/second
	float ttl;          // seconds
	float reload_time;  // seconds
};

class Bullet : public Weapon {
public:
	float creation_time;

	Bullet(Game *game, std::shared_ptr<Team> team, uint32_t creator_id, const GunDef &def);
	virtual void damage(Ship &ship);
	virtual const GunDef &get_def() { return static_cast<const GunDef&>(def); }
	virtual void tick();
};

}

#endif

