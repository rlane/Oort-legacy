// Copyright 2011 Rich Lane
#ifndef OORT_SIM_BEAM_H_
#define OORT_SIM_BEAM_H_

#include "sim/weapon.h"

namespace Oort {

class Game;

struct BeamDef : public WeaponDef {
	float damage;  // Watts
	float length;  // meters
	float width;   // meters
	glm::vec2 origin; // meters XXX put in WeaponDef
};

class Beam : public Weapon {
public:
	Beam(Game *game, std::shared_ptr<Team> team, uint32_t creator_id, const BeamDef &beam);
	virtual void damage(Ship &ship);
	virtual const BeamDef &get_def() { return static_cast<const BeamDef&>(def); }
};

}

#endif

