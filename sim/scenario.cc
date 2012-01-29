// Copyright 2011 Rich Lane
#include "sim/scenario.h"

#include <stdexcept>

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
	} else {
		throw std::runtime_error("Invalid scenario");
	}

	return scn;
}

}
