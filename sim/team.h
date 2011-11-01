// Copyright 2011 Rich Lane
#ifndef OORT_SIM_TEAM_H_
#define OORT_SIM_TEAM_H_

#include <string>
#include "glm/glm.hpp"

namespace Oort {

class Team {
public:
  std::string name;
  glm::vec3 color;

  Team(std::string name, glm::vec3 color)
    : name(name), color(color) {}
};

}

#endif
