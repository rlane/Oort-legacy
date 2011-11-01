// Copyright 2011 Rich Lane
#include "sim/ship.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include "glm/gtx/vector_angle.hpp"

#include "sim/physics.h"
#include "common/log.h"

using glm::vec2;
using glm::dvec2;

namespace Oort {

static boost::random::mt19937 prng(42);
static boost::random::normal_distribution<> a_dist(0.0, 10.0);

Ship::Ship(std::shared_ptr<Team> team)
	: team(team)	{
	//log("created ship\n");
}

Ship::~Ship() {
	//log("destroyed ship\n");
}

void Ship::tick() {
	physics.a = dvec2(a_dist(prng), a_dist(prng));
	auto norm_v = glm::normalize(dvec2(physics.v.x, -physics.v.y));
	physics.h = glm::radians(glm::orientedAngle(norm_v, dvec2(1, 0)));
}

}
