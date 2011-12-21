// Copyright 2011 Rich Lane
#ifndef OORT_SIM_MODEL_H_
#define OORT_SIM_MODEL_H_

#include <vector>
#include <string>
#include "glm/glm.hpp"

namespace Oort {

struct Shape {
	std::vector<glm::vec2> vertices;
};

struct Model {
	std::string name;
	Shape collision_shape;
	std::vector<Shape> shapes;
	float alpha;

	static Model *load(std::string name);
};

}

#endif
