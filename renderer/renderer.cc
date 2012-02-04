#include "renderer/renderer.h"
#include <boost/foreach.hpp>
#include <boost/units/detail/utility.hpp>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "gl/check.h"
#include "renderer/batches/ship.h"
#include "renderer/batches/tail.h"
#include "renderer/batches/bullet.h"
#include "renderer/batches/beam.h"
#include "renderer/batches/text.h"
#include "renderer/batches/particle.h"
#include "renderer/batches/clear.h"

using glm::vec2;
using glm::vec4;
using std::make_shared;
using std::shared_ptr;
using boost::units::detail::demangle;
using namespace Oort::RendererBatches;

namespace Oort {

Renderer::Renderer() {
	benchmark = false;
	add_batch<ClearBatch>();
	add_batch<TailBatch>();
	add_batch<BulletBatch>();
	add_batch<BeamBatch>();
	add_batch<ParticleBatch>();
	add_batch<ShipBatch>();
	add_batch<TextBatch>();
}

template <typename T>
void Renderer::add_batch() {
	batches.push_back(std::make_shared<T>(*this));
}

void Renderer::reshape(int screen_width, int screen_height) {
	this->screen_width = screen_width;
	this->screen_height = screen_height;
	this->aspect_ratio = float(screen_width)/screen_height;
}

void Renderer::render(float view_radius,
                      glm::vec2 view_center,
                      float time_delta) {
	Timer timer;
	GL::check();

#ifndef __native_client__
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(1.2f);

	p_matrix = glm::ortho(view_center.x - view_radius,
	                      view_center.x + view_radius,
	                      view_center.y - view_radius/aspect_ratio,
	                      view_center.y + view_radius/aspect_ratio);

	view_scale = screen_width/view_radius;

	BOOST_FOREACH(auto batch, batches) {
		//log("rendering batch %s", typeid(*batch).name());
		Timer timer;
		batch->render(time_delta);
		//GL::check();
		if (benchmark) {
			glFinish();
			batch->render_perf.update(timer);
		}
	}
	GL::check();

	texts.clear();

	if (benchmark) {
		render_perf.update(timer);
	}
}

void Renderer::tick(const Game &game) {
	Timer timer;
	BOOST_FOREACH(auto batch, batches) {
		//log("renderer ticking batch %s", typeid(*batch).name());
		Timer timer;
		batch->tick(game);
		if (benchmark) {
			batch->tick_perf.update(timer);
		}
	}
	if (benchmark) {
		tick_perf.update(timer);
	}
}

vec2 Renderer::pixel2screen(vec2 p) {
	return vec2((float) (2*p.x/screen_width-1),
	            (float) (-2*p.y/screen_height+1));
}

void Renderer::text(int x, int y, const std::string &str) {
	texts.emplace_back(Text{x, y, str});
}

void Renderer::dump_perf() {
	log("Renderer performance:");
	log("render   overall: %s", render_perf.summary().c_str());
	log("snapshot overall: %s", tick_perf.summary().c_str());
	BOOST_FOREACH(auto batch, batches) {
		auto name_str = demangle(typeid(*batch).name());
		auto name = strrchr(name_str.c_str(), ':') + 1;
		log("render   %13s %s", name, batch->render_perf.summary().c_str());
		log("snapshot %13s %s", name, batch->tick_perf.summary().c_str());
	}
	log("");
}

}
