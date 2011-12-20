// Copyright 2011 Rich Lane
#ifndef OORT_SIM_AI_LIB_H_
#define OORT_SIM_AI_LIB_H_

#include "glm/glm.hpp"

namespace Oort {

class Ship;

namespace AILib {

float smallest_positive_root_of_quadratic_equation(float a, float b, float c);
int accelerated_goto(float p, float v, float a);
void turn_to(Ship &s, float angle);
void drive_towards(Ship &s, glm::vec2 tp, float speed);

}
}

#endif
