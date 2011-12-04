// Copyright 2011 Rich Lane
#include "sim/ship.h"

#include <Box2D/Box2D.h>

#include "sim/ai.h"
#include "sim/team.h"
#include "sim/game.h"
#include "common/log.h"

using glm::vec2;

namespace Oort {

Ship::Ship(Game *game, std::shared_ptr<Team> team)
	: Entity(game),
	  team(team),
	  ai(new AI(this, team->ai)) {
	b2CircleShape shape;
	shape.m_radius = 1.0f;
	body->CreateFixture(&shape, 1.0f);
}

Ship::~Ship() {
}

void Ship::tick() {
	ai->tick();
}

}
