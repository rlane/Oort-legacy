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

struct ShipState {
	vec2 p;
	vec2 v;
	float h;
	float w;
	const ShipClass &klass;
	const Team &team;
};

struct ShipPriv {
	GL::Program prog;
	std::list<ShipState> ships;

	ShipPriv()
		: prog(GL::Program::from_resources("ship")) {}
};

ShipBatch::ShipBatch(Renderer &renderer)
	: Batch(renderer),
	  priv(make_shared<ShipPriv>()) {}

void ShipBatch::render(float time_delta) {
	auto &prog = priv->prog;
	glBlendFunc(GL_ONE, GL_ONE);
	prog.use();
	prog.enable_attrib_array("vertex");
	GL::check();
	prog.uniform("p_matrix", renderer.p_matrix);

	std::vector<vec2> jitters = {
#if 0
#warning "4 samples"
		vec2(0.375, 0.25),
		vec2(0.125, 0.75),
		vec2(0.875, 0.25),
		vec2(0.625, 0.75),
#elif 1
#warning "16 samples"
		vec2(0.375, 0.4375), vec2(0.625, 0.0625), vec2(0.875, 0.1875), vec2(0.125, 0.0625),
		vec2(0.375, 0.6875), vec2(0.875, 0.4375), vec2(0.625, 0.5625), vec2(0.375, 0.9375),
		vec2(0.625, 0.3125), vec2(0.125, 0.5625), vec2(0.125, 0.8125), vec2(0.375, 0.1875),
		vec2(0.875, 0.9375), vec2(0.875, 0.6875), vec2(0.125, 0.3125), vec2(0.625, 0.8125),
#else
#error "no samples"
#endif
	};

	BOOST_FOREACH(auto &ship, priv->ships) {
		glm::mat4 mv_matrix;
		auto p = ship.p + ship.v * time_delta;
		auto h = ship.h + ship.w * time_delta;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(p, 0));
		mv_matrix = glm::rotate(mv_matrix, glm::degrees(h), glm::vec3(0, 0, 1));
		mv_matrix = glm::scale(mv_matrix, glm::vec3(1, 1, 1) * ship.klass.scale);
		glm::vec4 color(ship.team.color, ship.klass.model->alpha/jitters.size());
		GL::check();

		prog.uniform("mv_matrix", mv_matrix);
		prog.uniform("color", color);

		BOOST_FOREACH(Shape &shape, ship.klass.model->shapes) {
			auto &vertex_buf = shape.vertex_buffer;
			if (!vertex_buf) {
				vertex_buf = std::make_shared<GL::Buffer>();
				vertex_buf->data(shape.vertices);
			}
			vertex_buf->bind();
			prog.attrib_ptr("vertex", (vec2*)NULL);
			vertex_buf->unbind();
			GL::check();

			BOOST_FOREACH(auto jitter, jitters) {
				prog.uniform("jitter", jitter*(4.0f/1600));
				glDrawArrays(GL_LINE_LOOP, 0, shape.vertices.size());
			}
			GL::check();
		}
	}

	prog.disable_attrib_array("vertex");
	GL::Program::clear();
	GL::check();
}

void ShipBatch::tick(const Game &game) {
	priv->ships.clear();
	BOOST_FOREACH(auto ship, game.ships) {
		if (ship->dead) {
			continue;
		}

		priv->ships.emplace_back(ShipState{
			ship->get_position(),
			ship->get_velocity(),
			ship->get_heading(),
			ship->get_angular_velocity(),
			ship->klass,
			*ship->team
		});
	}
}

}
}
