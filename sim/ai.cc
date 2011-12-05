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
#include <Box2D/Box2D.h>

#include "sim/game.h"
#include "sim/ship.h"
#include "sim/bullet.h"
#include "common/log.h"

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

float normalize_angle(float a) {
	while (a < -M_PI) a += 2*M_PI;
	while (a > M_PI) a -= 2*M_PI;
	return a;
}

float angle_diff(float a, float b) {
	float c = normalize_angle(b - a);
	if (c > M_PI) {
		c -= 2*M_PI;
	}
	return c;
}

float angle_between(b2Vec2 a, b2Vec2 b) {
	auto c = b - a;
	return atan2(c.y, c.x);
}

void AI::tick() {
	ship->thrust_main(10);
	ship->thrust_lateral(0);

	auto t = ship->body->GetTransform();
	auto w = ship->body->GetAngularVelocity();
	auto th = angle_between(t.p, b2Vec2(0,0));
	auto dh = angle_diff(t.q.GetAngle(), th);
	ship->thrust_angular(dh - w);

	if (ship->game->ticks % 4 == 0) {
		ship->fire(th);
	}
}

}
