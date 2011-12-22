// Copyright 2011 Rich Lane
#include "sim/beam.h"

#include <Box2D/Box2D.h>

#include "sim/ai.h"
#include "sim/team.h"
#include "sim/game.h"
#include "sim/math_util.h"
#include "sim/ship_class.h"
#include "common/log.h"

using glm::vec2;

namespace Oort {

Beam::Beam(Game *game,
           std::shared_ptr<Team> team,
           uint32_t creator_id,
           const BeamDef &def)
  : Entity(game, team),
    creator_id(creator_id) {
	damage = def.damage;
	width = def.width;
	length = def.length;
	std::vector<b2Vec2> vertices = {
		n2b(vec2(0, -def.width/2)),
		n2b(vec2(def.length, -def.width/2)),
		n2b(vec2(def.length, def.width/2)),
		n2b(vec2(0, def.width/2)),
	};
	b2PolygonShape shape;
	shape.Set(&vertices[0], vertices.size());
	body->CreateFixture(&shape, 0)->SetSensor(true);
}

Beam::~Beam() {
}

bool Beam::is_weapon() {
	return true;
}

}
