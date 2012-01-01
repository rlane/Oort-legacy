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

struct BulletState {
	vec2 p;
	vec2 v;
};

struct BulletPriv {
	GL::Program prog;
	std::vector<BulletState> bullets;

	BulletPriv()
		: prog(GL::Program::from_resources("bullet")) {}
};

BulletBatch::BulletBatch(Renderer &renderer)
	: Batch(renderer),
	  priv(make_shared<BulletPriv>())
{
}

void BulletBatch::tick() {
	priv->bullets.clear();
	BOOST_FOREACH(auto bullet, game.bullets) {
		if (bullet->dead || bullet->get_def().type == GunType::PLASMA) {
			continue;
		}

		priv->bullets.emplace_back(BulletState{
			bullet->get_position(),
			bullet->get_velocity()
		});
	}
}

void BulletBatch::render() {
	auto &prog = priv->prog;

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

	BOOST_FOREACH(auto &bullet, priv->bullets) {
		prog.attrib_ptr("color", colors);

		auto dp = bullet.v * (1.0f/40);
		auto p1 = bullet.p - dp;
		auto p2 = bullet.p;

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
