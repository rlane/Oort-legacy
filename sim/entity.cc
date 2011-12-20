// Copyright 2011 Rich Lane
#include "sim/entity.h"

#include <Box2D/Box2D.h>

#include "sim/ai.h"
#include "sim/team.h"
#include "sim/game.h"
#include "sim/math_util.h"
#include "common/log.h"

using glm::vec2;

namespace Oort {

Entity::Entity(Game *game, std::shared_ptr<Team> team)
	: game(game),
	  team(team),
	  dead(false),
	  mass(0) {
	b2BodyDef def;
	def.type = b2_dynamicBody;
	def.userData = this;
	body = game->world->CreateBody(&def);
}

Entity::~Entity() {
	game->world->DestroyBody(body);
}

void Entity::tick() {
	set_heading(normalize_angle(get_heading()));
}

void Entity::set_position(vec2 p) {
	body->SetTransform(n2b(p), body->GetAngle());
}

vec2 Entity::get_position() const {
	return b2n(body->GetPosition());
}

void Entity::set_velocity(vec2 p) {
	body->SetLinearVelocity(n2b(p));
}

vec2 Entity::get_velocity() const {
	return b2n(body->GetLinearVelocity());
}

void Entity::set_heading(float angle) {
	body->SetTransform(body->GetPosition(), angle);
}

float Entity::get_heading() const {
	return body->GetAngle();
}

void Entity::set_angular_velocity(float w) {
	body->SetAngularVelocity(w);
}

float Entity::get_angular_velocity() const {
	return body->GetAngularVelocity();
}

}
