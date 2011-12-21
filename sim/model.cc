#include "sim/model.h"

#include <boost/foreach.hpp>

namespace Oort {

Model *Model::load(std::string name) {
	auto model = new Model;
	model->name = "fighter";
	model->shapes.push_back(Shape());
	model->shapes[0].vertices = { glm::vec2(-0.7, -0.71), glm::vec2(1, 0), glm::vec2(-0.7, 0.71) };
	BOOST_FOREACH(glm::vec2 &v, model->shapes[0].vertices) { v *= 10; }
	model->collision_shape = model->shapes[0];
	return model;
}

}
