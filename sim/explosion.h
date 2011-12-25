// Copyright 2011 Rich Lane
#ifndef OORT_SIM_EXPLOSION_H_
#define OORT_SIM_EXPLOSION_H_

#include "glm/glm.hpp"

namespace Oort {

class Game;
class Team;

struct Explosion {
	Team *team;
	glm::vec2 p;
	float e;

	void tick(Game &game);
};

}

#endif
