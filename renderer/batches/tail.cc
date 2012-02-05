#include "renderer/batches/tail.h"

#include <memory>
#include <boost/foreach.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "sim/game.h"
#include "sim/ship.h"
#include "sim/ship_class.h"
#include "sim/team.h"
#include "gl/check.h"
#include "common/resources.h"
#include "renderer/bunch.h"

using glm::vec2;
using glm::vec4;
using std::make_shared;
using std::shared_ptr;

namespace Oort {
namespace RendererBatches {

struct TailVertex {
	glm::vec2 p;
	glm::vec4 color;
};

struct TailSegment {
	TailVertex a, b;
};

template<> std::vector<GLuint> Bunch<TailSegment>::buffer_freelist = std::vector<GLuint>();

struct TailPriv {
	GL::Program tail_prog;
	std::vector<TailSegment> tmp_segments;
	std::list<Bunch<TailSegment>> bunches;
	float time;

	TailPriv()
		: tail_prog(GL::Program::from_resources("tail"))
	{
	}

};

TailBatch::TailBatch(Renderer &renderer)
	: Batch(renderer),
	  priv(make_shared<TailPriv>())
{
}

void TailBatch::render(float time_delta) {
	auto &prog = priv->tail_prog;
	prog.use();
	prog.enable_attrib_array("vertex");
	prog.enable_attrib_array("color");
	prog.uniform("p_matrix", renderer.p_matrix);
	prog.uniform("current_time", priv->time + time_delta);
	int stride = sizeof(TailVertex);
	TailVertex *v = (TailVertex*)NULL;

	BOOST_FOREACH(auto &bunch, priv->bunches) {
		if (bunch.size == 0) {
			continue;
		}
		bunch.bind();
		prog.attrib("initial_time", bunch.initial_time);
		prog.attrib_ptr("vertex", &v->p, stride);
		prog.attrib_ptr("color", &v->color, stride);
		glDrawArrays(GL_LINES, 0, bunch.size*2);
	}
	Bunch<TailSegment>::unbind();

	prog.disable_attrib_array("vertex");
	prog.disable_attrib_array("color");
	GL::Program::clear();
}

void TailBatch::snapshot(const Game &game) {
	priv->time = game.time;

	if (priv->bunches.size() >= 128) {
		priv->bunches.pop_back();
	}

	BOOST_FOREACH(auto ship, game.ships) {
		priv->tmp_segments.emplace_back(
			TailSegment{
				TailVertex{ ship->get_position() - ship->get_velocity()*0.7f, vec4(ship->team->color, 0) },
				TailVertex{ ship->get_position(), vec4(ship->team->color, ship->klass.tail_alpha) }
			}
		);
	}

	priv->bunches.push_front(Bunch<TailSegment>(game.time, std::move(priv->tmp_segments)));
}

}
}
