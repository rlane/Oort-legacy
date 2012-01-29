// Copyright 2011 Rich Lane
#include "sim/scenario.h"

namespace Oort {

using namespace glm;

Scenario::Scenario() {
}

Scenario Scenario::load(std::string path) {
	Scenario scn;
	scn.teams.emplace_back();
	auto red = scn.teams.back();
	red.name = "red";
	red.color = vec3(1, 0, 0);
	{
		red.ships.emplace_back();
		auto s = red.ships.back();
		s.klass = "fighter";
		s.p = vec2(0, 0);
		s.v = vec2(0, 0);
		s.h = 0;
	}
	return scn;
}

}
