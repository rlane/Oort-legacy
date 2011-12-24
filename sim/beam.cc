// Copyright 2011 Rich Lane
#include "sim/beam.h"

#include <Box2D/Box2D.h>

#include "sim/ai.h"
#include "sim/team.h"
#include "sim/game.h"
#include "sim/math_util.h"
#include "sim/ship_class.h"
#include "sim/ship.h"
#include "common/log.h"

using glm::vec2;

namespace Oort {

Beam::Beam(Game *game,
           std::shared_ptr<Team> team,
					 uint32_t creator_id,
           const BeamDef &def)
  : Weapon(game, team, creator_id, def) {
	std::vector<b2Vec2> vertices = {
		n2b(vec2(0, -def.width/2)),
		n2b(vec2(def.length, -def.width/2)),
		n2b(vec2(def.length, def.width/2)),
		n2b(vec2(0, def.width/2)),
	};
	b2PolygonShape shape;
	shape.Set(&vertices[0], vertices.size());
	body->CreateFixture(&shape, 0.1)->SetSensor(true);
}

float Beam::damage(const Ship &ship) {
	constexpr float tick_length = 1.0/32; // XXX
	float e = get_def().damage * tick_length;
	//printf("ship %d; beam %p; damage %g\n", ship.id, this, e);
	return e;
}

}
