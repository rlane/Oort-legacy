// Copyright 2011 Rich Lane
#ifndef OORT_SIM_TEAM_H_
#define OORT_SIM_TEAM_H_

#include <string>
#include <memory>
#include "glm/glm.hpp"
#include "sim/ai.h"

namespace Oort {

class Team {
public:
  std::string name;
	std::shared_ptr<AIFactory> ai_factory;
  glm::vec3 color;

  Team(std::string name, std::shared_ptr<AIFactory> ai_factory, glm::vec3 color)
    : name(name), ai_factory(ai_factory), color(color) {}
};

}

#endif
