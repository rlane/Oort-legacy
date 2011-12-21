#include "sim/model.h"

#include <boost/foreach.hpp>
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
		s.vertices.push_back(glm::vec2(x, y) * 10.0f);
	}
	return s;
}

Model *Model::load(std::string name) {
	auto data = load_resource("models/" + name + ".json");
	json_spirit::mValue value;
	json_spirit::read_string(data, value);
	json_spirit::mObject &obj = value.get_obj();

	auto model = new Model;
	model->name = obj.find("name")->second.get_str();
	json_spirit::mArray shapes = obj.find("shapes")->second.get_array();
	BOOST_FOREACH(json_spirit::mValue &e, shapes) {
		model->shapes.push_back(read_shape(e.get_array()));
	}
	
	model->collision_shape = model->shapes[0];
	return model;
}

}
