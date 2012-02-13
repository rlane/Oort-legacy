// Copyright 2011 Rich Lane
#ifndef OORT_SIM_BULLET_H_
#define OORT_SIM_BULLET_H_

#include "sim/weapon.h"

namespace Oort {

class Game;

enum class GunType {
	SLUG,
	PLASMA,
};

struct GunDef : public WeaponDef {
	GunType type;
	float mass;         // kg
	float velocity;     // meters/second
	float ttl;          // seconds
	float reload_time;  // seconds
	float deviation;    // radians
};

class Bullet {
public:
	std::shared_ptr<Team> team;
	const GunDef &def;
	float creation_time;
	uint32_t creator_id;
	glm::vec2 initial_position;
	glm::vec2 velocity;
	bool dead;

	Bullet(Game *game, std::shared_ptr<Team> team, uint32_t creator_id,
			   const GunDef &def, glm::vec2 p, glm::vec2 v);

	glm::vec2 get_position(float time) const {
		auto dt = time - creation_time;
		return initial_position + velocity*dt;
	}

	virtual float damage(const Ship &ship) const;

	static void tick_all(Game &Game);
};

}

#endif

