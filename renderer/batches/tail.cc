#include "renderer/batches/tail.h"

#include <memory>
#include <boost/foreach.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "sim/game.h"
#include "sim/ship.h"
#include "sim/team.h"
#include "gl/check.h"
#include "common/resources.h"

using glm::vec2;
using glm::vec4;
using std::make_shared;
using std::shared_ptr;

namespace Oort {
namespace RendererBatches {

TailBatch::TailBatch(Renderer &renderer)
	: Batch(renderer),
    prog(GL::Program(
      make_shared<GL::VertexShader>(load_resource("shaders/bullet.v.glsl")),
      make_shared<GL::FragmentShader>(load_resource("shaders/bullet.f.glsl"))))
{
}

void TailBatch::render() {
	prog.use();
	GL::check();

	prog.enable_attrib_array("vertex");
	prog.enable_attrib_array("color");
	prog.uniform("p_matrix", renderer.p_matrix);
	prog.uniform("mv_matrix", glm::mat4());
	int stride = sizeof(TailVertex);
	prog.attrib_ptr("vertex", &tail_segments[0].a.p, stride);
	prog.attrib_ptr("color", &tail_segments[0].a.color, stride);
	glDrawArrays(GL_LINES, 0, tail_segments.size()*2);
	prog.disable_attrib_array("vertex");
	prog.disable_attrib_array("color");
	GL::Program::clear();
	GL::check();
}

static bool tail_segment_expired(const TailSegment &ts) {
	return ts.b.color.a <= 0.0f;
}

void TailBatch::tick() {
	BOOST_FOREACH(auto &ts, tail_segments) {
		auto &alpha = ts.b.color.a;
		alpha = fmaxf(0, alpha - 0.02/32.0f);
	}

	tail_segments.erase(
		std::remove_if(begin(tail_segments), end(tail_segments), tail_segment_expired),
		end(tail_segments));

	BOOST_FOREACH(auto ship, game.ships) {
		tail_segments.emplace_back(
			TailSegment{
				TailVertex{ ship->get_position() - ship->get_velocity()*0.7f, vec4(ship->team->color, 0) },
				TailVertex{ ship->get_position(), vec4(ship->team->color, 0.1) }
			}
		);
	}
}

}
}
