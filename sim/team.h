// Copyright 2011 Rich Lane
#ifndef OORT_SIM_TEAM_H_
#define OORT_SIM_TEAM_H_

#include <string>
#include "glm/glm.hpp"
#include "sim/ai.h"

namespace Oort {

class Team {
public:
  std::string name;
  AISourceCode ai;
  glm::vec3 color;

  Team(std::string name, AISourceCode ai, glm::vec3 color)
    : name(name), ai(ai), color(color) {}
};

}

#endif
