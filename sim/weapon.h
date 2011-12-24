// Copyright 2011 Rich Lane
#ifndef OORT_SIM_WEAPON_H_
#define OORT_SIM_WEAPON_H_

#include <stdint.h>
#include <memory>
#include "sim/entity.h"

namespace Oort {

class Game;
class Ship;

struct WeaponDef {
	float angle;       // radians
	float coverage;    // radians
	glm::vec2 origin;  // meters
};

class Weapon : public Entity {
public:
	uint32_t creator_id;

	Weapon(Game *game,
	       std::shared_ptr<Team> team,
	       uint32_t creator_id,
	       const WeaponDef &def)
	  : Entity(game, team),
	    creator_id(creator_id),
	    def(def) {}
	virtual bool is_weapon() { return true; }
	virtual float damage(const Ship &ship) = 0;
	virtual const WeaponDef &get_def() = 0;

protected:
	const WeaponDef &def;
};

}

#endif
