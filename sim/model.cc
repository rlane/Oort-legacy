#include "sim/model.h"

#include <boost/foreach.hpp>
#include <Box2D/Box2D.h>
#include "json_spirit_reader_template.h"
#include "json_spirit_reader.h"
#include "common/resources.h"

namespace Oort {

static Shape read_shape(json_spirit::mArray &vertices) {
	Shape s;
	BOOST_FOREACH(json_spirit::mValue &e, vertices) {
		json_spirit::mObject obj = e.get_obj();
		float x = float(obj["x"].get_real());
		float y = float(obj["y"].get_real());
		s.vertices.push_back(glm::vec2(x, y));
	}
	s.vertex_buffer = nullptr;
	return s;
}

Model *Model::load(std::string name) {
	auto data = load_resource("models/" + name + ".json");
	json_spirit::mValue value;
	json_spirit::read_string(data, value);
	json_spirit::mObject &obj = value.get_obj();

	auto model = new Model;
	model->name = obj.find("name")->second.get_str();
	model->alpha = float(obj.find("alpha")->second.get_real());
	json_spirit::mArray shapes = obj.find("shapes")->second.get_array();
	BOOST_FOREACH(json_spirit::mValue &e, shapes) {
		model->shapes.push_back(read_shape(e.get_array()));
	}
	
	if (obj.count("collision_shape")) {
		json_spirit::mArray vertices = obj.find("collision_shape")->second.get_array();
		model->collision_shape = read_shape(vertices);
	} else {
		model->collision_shape = model->shapes[0];
	}

	// move center of mass to local origin
	std::vector<glm::vec2> &vertices = model->collision_shape.vertices;
	b2PolygonShape shape;
	shape.Set((b2Vec2*) &vertices[0], vertices.size());
	b2MassData md;
	shape.ComputeMass(&md, 1);
	glm::vec2 offset(md.center.x, md.center.y);
	BOOST_FOREACH(glm::vec2 &v, vertices) {
		v -= offset;
	}
	BOOST_FOREACH(Shape &shape, model->shapes) {
		BOOST_FOREACH(glm::vec2 &v, shape.vertices) {
			v -= offset;
		}
	}

	return model;
}

}
