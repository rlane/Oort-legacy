#include "renderer/batches/ship.h"

#include <memory>
#include <boost/foreach.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "sim/game.h"
#include "sim/ship.h"
#include "sim/ship_class.h"
#include "sim/model.h"
#include "sim/team.h"
#include "gl/buffer.h"
#include "gl/check.h"
#include "common/resources.h"

using glm::vec2;
using glm::vec4;
using std::make_shared;
using std::shared_ptr;

namespace Oort {
namespace RendererBatches {

ShipBatch::ShipBatch(Renderer &renderer)
	: Batch(renderer),
    prog(GL::Program(
      make_shared<GL::VertexShader>(load_resource("shaders/ship.v.glsl")),
      make_shared<GL::FragmentShader>(load_resource("shaders/ship.f.glsl"))))
{
}

void ShipBatch::render() {
	prog.use();
	prog.enable_attrib_array("vertex");
	GL::check();
	prog.uniform("p_matrix", renderer.p_matrix);

	BOOST_FOREACH(auto ship, game.ships) {
		if (ship->dead) {
			continue;
		}

		glm::mat4 mv_matrix;
		auto p = ship->get_position();
		auto h = ship->get_heading();
		mv_matrix = glm::translate(mv_matrix, glm::vec3(p.x, p.y, 0));
		mv_matrix = glm::rotate(mv_matrix, glm::degrees(h), glm::vec3(0, 0, 1));
		mv_matrix = glm::scale(mv_matrix, glm::vec3(1, 1, 1) * ship->klass.scale);
		glm::vec4 color(ship->team->color, ship->klass.model->alpha);
		GL::check();

		prog.uniform("mv_matrix", mv_matrix);
		prog.uniform("color", color);

		BOOST_FOREACH(Shape &shape, ship->klass.model->shapes) {
			auto &vertex_buf = shape.vertex_buffer;
			if (!vertex_buf) {
				vertex_buf = std::make_shared<GL::Buffer>();
				vertex_buf->data(shape.vertices);
			}
			vertex_buf->bind();
			prog.attrib_ptr("vertex", (vec2*)NULL);
			vertex_buf->unbind();
			GL::check();

			glDrawArrays(GL_LINE_LOOP, 0, shape.vertices.size());
			GL::check();
		}
	}

	prog.disable_attrib_array("vertex");
	GL::Program::clear();
	GL::check();
}

}
}
