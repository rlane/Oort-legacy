// Copyright 2011 Rich Lane
#ifndef OORT_SIM_SCENARIO_H_
#define OORT_SIM_SCENARIO_H_

#include <vector>
#include <memory>
#include <boost/optional.hpp>
#include "glm/glm.hpp"
#include "sim/game.h"

namespace Oort {

struct ScnShip {
	std::string klass;
	glm::vec2 p, v;
	float h;
};

struct ScnTeam {
	std::string name;
	glm::vec3 color;
	boost::optional<std::string> code;
	std::vector<ScnShip> ships;
};

class Scenario {
public:
	std::string description;
	std::string author;
	std::vector<ScnTeam> teams;

	Scenario();

	static Scenario load(std::string path);

private:
	static void load_json(Scenario &scn, std::string path);
};

}

#endif
