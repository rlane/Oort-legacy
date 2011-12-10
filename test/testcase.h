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
#include "sim/test.h"
#include "sim/ai.h"
#include "sim/team.h"
#include "sim/scenario.h"
#include "sim/ship.h"
#include "sim/math_util.h"

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

class Waypoint : public Entity {
public:
	Waypoint(Game *game, vec2 pos, float radius)
	  : Entity(game, NULL) {
		body->SetTransform(b2Vec2(pos.x, pos.y), 0);
		b2CircleShape shape;
		shape.m_radius = radius;
		b2FixtureDef def;
		def.shape = &shape;
		def.isSensor = true;
		body->CreateFixture(&def);
	}
};

void assert_contact(const Entity &a, const Entity &b) {
	auto contact = a.body->GetContactList();
	auto other = b.body;
	while (contact != NULL) {
		if (contact->other == other) {
			return;
		}
		contact = contact->next;
	}
	ostringstream msg;
	msg << "no contact: a=" << a.body->GetPosition() << " b=" << b.body->GetPosition();
	throw runtime_error(msg.str());
}
