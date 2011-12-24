#include "renderer/batches/bullet.h"

#include <memory>
#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "sim/game.h"
#include "sim/bullet.h"
#include "gl/check.h"
#include "common/resources.h"

using glm::vec2;
using glm::vec4;
using std::make_shared;
using std::shared_ptr;

namespace Oort {
namespace RendererBatches {

BulletBatch::BulletBatch(Renderer &renderer)
	: Batch(renderer),
    prog(GL::Program(
      make_shared<GL::VertexShader>(load_resource("shaders/bullet.v.glsl")),
      make_shared<GL::FragmentShader>(load_resource("shaders/bullet.f.glsl"))))
{
}

void BulletBatch::render() {
	vec4 colors[] = {
		vec4(0.27f, 0.27f, 0.27f, 0.33f),
		vec4(0.27f, 0.27f, 0.27f, 1.0f)
	};

	prog.use();
	GL::check();

	prog.enable_attrib_array("vertex");
	prog.enable_attrib_array("color");
	prog.uniform("p_matrix", renderer.p_matrix);
	prog.uniform("mv_matrix", glm::mat4());

	BOOST_FOREACH(auto bullet, game.bullets) {
		if (bullet->dead || bullet->get_def().type == GunType::PLASMA) {
			continue;
		}

		prog.attrib_ptr("color", colors);

		auto dp = bullet->get_velocity() * (1.0f/40);
		auto p1 = bullet->get_position() - dp;
		auto p2 = bullet->get_position();

		vec2 vertices[] = { p1, p2 };
		prog.attrib_ptr("vertex", vertices);
		glDrawArrays(GL_LINES, 0, 2);
	}

	prog.disable_attrib_array("vertex");
	prog.disable_attrib_array("color");
	GL::Program::clear();
	GL::check();
}

}
}
