// Copyright 2011 Rich Lane
#ifndef OORT_SIM_MODEL_H_
#define OORT_SIM_MODEL_H_

#include <vector>
#include <string>
#include <memory>
#include "glm/glm.hpp"

namespace GL {
	class Buffer;
}

namespace Oort {

struct Shape {
	std::vector<glm::vec2> vertices;
	std::shared_ptr<GL::Buffer> vertex_buffer;
};

struct Model {
	std::string name;
	Shape collision_shape;
	std::vector<Shape> shapes;
	float alpha;

	static std::shared_ptr<Model> load(std::string name);
};

}

#endif
