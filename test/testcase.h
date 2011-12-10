// Copyright 2011 Rich Lane
#include <memory>
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/foreach.hpp>
#include <exception>
#include <sstream>

#include "glm/glm.hpp"

#include "sim/game.h"
#include "sim/ai.h"
#include "sim/team.h"
#include "sim/scenario.h"
#include "sim/ship.h"
#include "sim/math_util.h"

extern "C" {
	Oort::Game *test_init(void);
	void test_tick(Oort::Game*);
}

using namespace std;
using namespace Oort;
using namespace glm;

static inline ostream& operator<<(ostream& s, const b2Vec2& a) {
	s << "(" << a.x << ", " << a.y << ")";
	return s;
}

static const float epsilon = 0.2;

static inline void assert_equal(float a, float b) {
	if (a > b + epsilon || a < b - epsilon) {
		ostringstream tmp;
		tmp << a << " != " << b;
		throw runtime_error(tmp.str());
	}
}

static inline void assert_equal(b2Vec2 a, b2Vec2 b) {
	try {
		assert_equal(a.x, b.x);
		assert_equal(a.y, b.y);
	} catch (runtime_error const &e) {
		ostringstream tmp;
		tmp << a << " != " << b;
		throw runtime_error(tmp.str());
	}
}
