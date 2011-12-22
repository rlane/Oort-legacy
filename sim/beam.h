// Copyright 2011 Rich Lane
#ifndef OORT_SIM_BEAM_H_
#define OORT_SIM_BEAM_H_

#include <stdint.h>
#include <memory>
#include "sim/entity.h"

namespace Oort {

class Game;
class BeamDef;

class Beam : public Entity {
	public:
	uint32_t creator_id;
	float damage;
	float width;
	float length;

	Beam(Game *game, std::shared_ptr<Team> team, uint32_t creator_id, const BeamDef &beam);
	~Beam();

	virtual bool is_weapon();
};

}

#endif

