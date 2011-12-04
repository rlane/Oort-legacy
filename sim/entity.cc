// Copyright 2011 Rich Lane
#include "sim/entity.h"

#include <Box2D/Box2D.h>

#include "sim/ai.h"
#include "sim/team.h"
#include "sim/game.h"
#include "common/log.h"

using glm::vec2;

namespace Oort {

Entity::Entity(Game *game, std::shared_ptr<Team> team)
	: game(game),
	  team(team),
	  dead(false) {
	b2BodyDef def;
	def.type = b2_dynamicBody;
	def.userData = this;
	body = game->world->CreateBody(&def);
}

Entity::~Entity() {
}

void Entity::tick() {
}

}
