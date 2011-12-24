#include "renderer/renderer.h"
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <stdint.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include <Box2D/Box2D.h>

#include "sim/ship.h"
#include "sim/ship_class.h"
#include "sim/bullet.h"
#include "sim/beam.h"
#include "sim/team.h"
#include "sim/model.h"
#include "sim/math_util.h"
#include "common/log.h"
#include "common/resources.h"
#include "gl/program.h"
#include "gl/shader.h"
#include "gl/buffer.h"
#include "gl/texture.h"
#include "gl/check.h"
#include "renderer/batches/ship.h"
#include "renderer/batches/tail.h"
#include "renderer/batches/bullet.h"
#include "renderer/batches/beam.h"
#include "renderer/batches/text.h"

using glm::vec2;
using glm::vec4;
using std::make_shared;
using std::shared_ptr;
using boost::scoped_ptr;
using namespace Oort::RendererBatches;

namespace Oort {

Renderer::Renderer(Game &game)
  : game(game)
{
	batches = {
		new TailBatch(*this),
		new ShipBatch(*this),
		new BulletBatch(*this),
		new BeamBatch(*this),
		new TextBatch(*this),
	};
}

void Renderer::reshape(int screen_width, int screen_height) {
	this->screen_width = screen_width;
	this->screen_height = screen_height;
	this->aspect_ratio = float(screen_width)/screen_height;
}

void Renderer::render(float view_radius,
                      glm::vec2 view_center) {
	GL::check();

	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.03f, 0.0f);
	glLineWidth(1.2f);

	glClear(GL_COLOR_BUFFER_BIT);

	p_matrix = glm::ortho(view_center.x - view_radius,
	                      view_center.x + view_radius,
	                      view_center.y - view_radius/aspect_ratio,
	                      view_center.y + view_radius/aspect_ratio);

	BOOST_FOREACH(auto batch, batches) {
		batch->render();
	}

	texts.clear();
}

void Renderer::tick() {
	BOOST_FOREACH(auto batch, batches) {
		batch->tick();
	}
}

vec2 Renderer::pixel2screen(vec2 p) {
	return vec2((float) (2*p.x/screen_width-1),
	            (float) (-2*p.y/screen_height+1));
}

void Renderer::text(int x, int y, const std::string &str) {
	texts.emplace_back(Text{x, y, str});
}

}
