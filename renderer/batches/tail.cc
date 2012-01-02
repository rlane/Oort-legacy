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

struct TailPriv {
	GL::Program prog;
	std::vector<TailSegment> tail_segments;

	TailPriv()
		: prog(GL::Program::from_resources("bullet")) {}

};

TailBatch::TailBatch(Renderer &renderer)
	: Batch(renderer),
	  priv(make_shared<TailPriv>())
{
}

void TailBatch::render() {
	auto &prog = priv->prog;
	prog.use();
	GL::check();

	prog.enable_attrib_array("vertex");
	prog.enable_attrib_array("color");
	prog.uniform("p_matrix", renderer.p_matrix);
	prog.uniform("mv_matrix", glm::mat4());
	int stride = sizeof(TailVertex);
	TailVertex &v = priv->tail_segments[0].a;
	prog.attrib_ptr("vertex", &v.p, stride);
	prog.attrib_ptr("color", &v.color, stride);
	glDrawArrays(GL_LINES, 0, priv->tail_segments.size()*2);
	prog.disable_attrib_array("vertex");
	prog.disable_attrib_array("color");
	GL::Program::clear();
	GL::check();
}

static bool tail_segment_expired(const TailSegment &ts) {
	return ts.b.color.a <= 0.0f;
}

void TailBatch::tick() {
	BOOST_FOREACH(auto &ts, priv->tail_segments) {
		auto &alpha = ts.b.color.a;
		alpha = fmaxf(0, alpha - 0.02*Game::tick_length);
	}

	priv->tail_segments.erase(
		std::remove_if(priv->tail_segments.begin(), priv->tail_segments.end(), tail_segment_expired),
		priv->tail_segments.end());

	BOOST_FOREACH(auto ship, game.ships) {
		priv->tail_segments.emplace_back(
			TailSegment{
				TailVertex{ ship->get_position() - ship->get_velocity()*0.7f, vec4(ship->team->color, 0) },
				TailVertex{ ship->get_position(), vec4(ship->team->color, ship->klass.tail_alpha) }
			}
		);
	}
}

}
}
