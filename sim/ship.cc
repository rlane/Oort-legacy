// Copyright 2011 Rich Lane
#include "sim/ship.h"

#include "sim/physics.h"
#include "sim/ai.h"
#include "common/log.h"

using glm::vec2;
using glm::dvec2;

namespace Oort {

Ship::Ship(std::shared_ptr<Team> team)
	: team(team),
	  ai(new AI(this, team->ai)) {
}

Ship::~Ship() {
}

void Ship::tick() {
	ai->tick();
}

}
