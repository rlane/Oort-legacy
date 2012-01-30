// Copyright 2011 Rich Lane
#include "sim/scenario.h"

#include <stdexcept>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>

namespace Oort {

using namespace glm;

Scenario::Scenario() {
}

Scenario Scenario::load(std::string path) {
	Scenario scn;

	if (path == "test/beam.json") {
		{
			scn.teams.emplace_back();
			auto &team = scn.teams.back();
			team.name = "green";
			team.color = vec3(0, 1, 0);
			{
				team.ships.emplace_back();
				auto &s = team.ships.back();
				s.klass = "ion_cannon_frigate";
				s.p = vec2(-600, 0);
				s.v = vec2(0, 0);
				s.h = 0;
			}
		}

		{
			scn.teams.emplace_back();
			auto &team = scn.teams.back();
			team.name = "red";
			team.color = vec3(1, 0, 0);
			{
				team.ships.emplace_back();
				auto &s = team.ships.back();
				s.klass = "fighter";
				s.p = vec2(600, 0);
				s.v = vec2(0, 0);
				s.h = 3.14159;
			}
		}
	} else if (path == "test/chase.json") {
		{
			scn.teams.emplace_back();
			auto &team = scn.teams.back();
			team.name = "green";
			team.color = vec3(0, 1, 0);
			{
				team.ships.emplace_back();
				auto &s = team.ships.back();
				s.klass = "fighter";
				s.p = vec2(0, 0);
				s.v = vec2(0, 0);
				s.h = 0;
			}
		}

		{
			scn.teams.emplace_back();
			auto &team = scn.teams.back();
			team.name = "red";
			team.color = vec3(1, 0, 0);
			{
				team.ships.emplace_back();
				auto &s = team.ships.back();
				s.klass = "target";
				s.p = vec2(1500, 0);
				s.v = vec2(0, 0);
				s.h = 0;
			}
		}
	} else if (path == "test/furball.json") {
		boost::random::mt19937 prng(42);
		boost::random::normal_distribution<float> p_dist(0.0, 1000.0);
		boost::random::normal_distribution<float> v_dist(0.0, 20.0);
		boost::uniform_real<float> h_dist(0.0, 2*3.14159);

		{
			scn.teams.emplace_back();
			auto &team = scn.teams.back();
			team.name = "red";
			team.color = vec3(1, 0, 0);
		}

		{
			scn.teams.emplace_back();
			auto &team = scn.teams.back();
			team.name = "green";
			team.color = vec3(0, 1, 0);
		}

		{
			scn.teams.emplace_back();
			auto &team = scn.teams.back();
			team.name = "blue";
			team.color = vec3(0, 0, 1);
		}

		std::vector<std::string> klasses = { 
			"fighter",
			"fighter",
			"fighter",
			"missile",
			"ion_cannon_frigate",
			"assault_frigate",
		};

		for (auto i = 0; i < 100; i++) {
			auto &team = scn.teams[i % scn.teams.size()];
			auto &klass = klasses[(i/scn.teams.size()) % klasses.size()];
			team.ships.emplace_back();
			auto &s = team.ships.back();
			s.klass = klass;
			s.p = vec2(p_dist(prng), p_dist(prng));
			s.v = vec2(v_dist(prng), v_dist(prng));
			s.h = h_dist(prng);
		}
	} else if (path == "test/gun.json") {
		{
			scn.teams.emplace_back();
			auto &team = scn.teams.back();
			team.name = "green";
			team.color = vec3(0, 1, 0);
			{
				team.ships.emplace_back();
				auto &s = team.ships.back();
				s.klass = "fighter";
				s.p = vec2(0, 0);
				s.v = vec2(0, 0);
				s.h = 0;
			}
		}

		{
			scn.teams.emplace_back();
			auto &team = scn.teams.back();
			team.name = "red";
			team.color = vec3(1, 0, 0);
			{
				team.ships.emplace_back();
				auto &s = team.ships.back();
				s.klass = "ion_cannon_frigate";
				s.p = vec2(500, 0);
				s.v = vec2(0, 0);
				s.h = 1.57;
			}
		}
	} else {
		throw std::runtime_error("Invalid scenario");
	}

	return scn;
}

}
