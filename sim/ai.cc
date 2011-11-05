// Copyright 2011 Rich Lane
#include "sim/ai.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "glm/glm.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "sim/ship.h"
#include "common/log.h"

using glm::dvec2;

namespace Oort {

static boost::random::mt19937 prng(42);
static boost::random::normal_distribution<> a_dist(0.0, 10.0);

AI::AI(Ship *ship, AISourceCode &src)
	: ship(ship) {
	G = lua_open();
}

AI::~AI() {
	if (G) {
		lua_close(G);
	}
}

void AI::tick() {
	ship->physics.a = dvec2(a_dist(prng), a_dist(prng));
	auto norm_v = glm::normalize(dvec2(ship->physics.v.x, -ship->physics.v.y));
	ship->physics.h = glm::radians(glm::orientedAngle(norm_v, dvec2(1, 0)));
}

}
