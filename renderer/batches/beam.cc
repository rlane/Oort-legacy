#include "renderer/batches/beam.h"

#include <memory>
#include <boost/foreach.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "sim/game.h"
#include "sim/beam.h"
#include "gl/buffer.h"
#include "gl/check.h"
#include "common/resources.h"

using glm::vec2;
using glm::vec4;
using std::make_shared;
using std::shared_ptr;

namespace Oort {
namespace RendererBatches {

struct BeamPriv {
	GL::Program prog;

	BeamPriv()
		: prog(GL::Program(
		        make_shared<GL::VertexShader>(load_resource("shaders/beam.v.glsl")),
		        make_shared<GL::FragmentShader>(load_resource("shaders/beam.f.glsl")))) {}
};

BeamBatch::BeamBatch(Renderer &renderer)
	: Batch(renderer),
	  priv(make_shared<BeamPriv>()) {}

void BeamBatch::render() {
	auto &prog = priv->prog;
	prog.use();
	prog.uniform("p_matrix", renderer.p_matrix);
	prog.enable_attrib_array("vertex");
	prog.enable_attrib_array("texcoord");

	vec2 texcoords[] = {
		vec2(0, 1),
		vec2(0, 0),
		vec2(1, 1),
		vec2(1, 0)
	};

	prog.attrib_ptr("texcoord", texcoords);

	BOOST_FOREACH(auto beam, game.beams) {
		glm::vec4 color = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);

		glm::mat4 mv_matrix;
		auto p = beam->get_position();
		auto h = beam->get_heading();
		mv_matrix = glm::translate(mv_matrix, glm::vec3(p.x, p.y, 0));
		mv_matrix = glm::rotate(mv_matrix, glm::degrees(h), glm::vec3(0, 0, 1));

		auto &def = beam->get_def();
		vec2 vertices[] = {
			vec2(0, def.width/2.0f),
			vec2(0, -def.width/2.0f),
			vec2(def.length, def.width/2.0f),
			vec2(def.length, -def.width/2.0f)
		};

		prog.uniform("mv_matrix", mv_matrix);
		prog.uniform("color", color);

		prog.attrib_ptr("vertex", vertices);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	prog.disable_attrib_array("vertex");
	prog.disable_attrib_array("texcoord");
	GL::Program::clear();
}

}
}
