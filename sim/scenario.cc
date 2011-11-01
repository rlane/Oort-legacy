// Copyright 2011 Rich Lane
#include "sim/scenario.h"

#include <memory>
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>

#include "glm/glm.hpp"

namespace Oort {

Scenario::Scenario() {
}

void Scenario::setup(Game &game) {
	boost::random::mt19937 prng(42);
	boost::random::normal_distribution<> p_dist(0.0, 1.0);
	boost::random::normal_distribution<> v_dist(0.0, 5.0);

	std::vector<std::shared_ptr<Team>> teams = {
		std::make_shared<Team>("red", glm::vec3(1, 0, 0)),
		std::make_shared<Team>("green", glm::vec3(0, 1, 0)),
		std::make_shared<Team>("blue", glm::vec3(0, 0, 1)),
	};

	for (auto i = 0; i < 100; i++) {
		auto ship = std::make_shared<Ship>(teams[i%teams.size()]);
		ship->physics.p = glm::dvec2(p_dist(prng), p_dist(prng));
		ship->physics.v = glm::dvec2(v_dist(prng), v_dist(prng));
		ship->physics.h = 0.0;
		game.ships.push_back(ship);
	}
}

}
