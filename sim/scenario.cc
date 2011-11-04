// Copyright 2011 Rich Lane
#include "sim/scenario.h"

#include <memory>
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>

#include "glm/glm.hpp"

namespace Oort {

Scenario::Scenario() {
	boost::random::mt19937 prng(42);
	boost::random::normal_distribution<> p_dist(0.0, 1.0);
	boost::random::normal_distribution<> v_dist(0.0, 5.0);

	teams = {
		{ "red", glm::vec3(1, 0, 0), nullptr, {} },
		{ "green", glm::vec3(0, 1, 0), nullptr, {} },
		{ "blue", glm::vec3(0, 0, 1), nullptr, {} },
	};

	for (auto i = 0; i < 100; i++) {
		ScnShip ship;
		ship.klass = "fighter";
		ship.p = glm::dvec2(p_dist(prng), p_dist(prng));
		ship.v = glm::dvec2(v_dist(prng), v_dist(prng));
		ship.h = 0.0;
		teams[i % teams.size()].ships.push_back(ship);
	}
}

}
