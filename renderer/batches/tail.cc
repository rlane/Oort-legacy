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

struct FramebufferTexture {
	GLuint tex;
	GLuint fbo;

	FramebufferTexture() {
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
		{
			GLenum status;
			status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
			switch (status) {
				case GL_FRAMEBUFFER_COMPLETE:
					break;
				case GL_FRAMEBUFFER_UNSUPPORTED:
					log("Error: unsupported framebuffer format");
					abort();
				default:
					log("Error: invalid framebuffer config");
					abort();
			}
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		GL::check();
	}
};

struct TailVertex {
	glm::vec2 p;
	glm::vec4 color;
};

struct TailSegment {
	TailVertex a, b;
};

struct TailPriv {
	GL::Program tail_prog;
	GL::Program horiz_blur_prog;
	GL::Program vert_blur_prog;
	FramebufferTexture fbs[2];
	std::vector<TailSegment> tail_segments;

	TailPriv()
		: tail_prog(GL::Program::from_resources("bullet")),
		  horiz_blur_prog(GL::Program::from_resources("gaussian", "gaussian_horiz")),
		  vert_blur_prog(GL::Program::from_resources("gaussian", "gaussian_vert"))
	{
	}

};

TailBatch::TailBatch(Renderer &renderer)
	: Batch(renderer),
	  priv(make_shared<TailPriv>())
{
}

void TailBatch::render(float time_delta) {
	std::vector<vec2> fsquad_vertices = { vec2(1,0), vec2(0,0), vec2(1,1), vec2(0,1) };

	// create fb textures
	// TODO only recreate if dimensions changed
	{
		glBindTexture(GL_TEXTURE_2D, priv->fbs[0].tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, renderer.screen_width, renderer.screen_height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, priv->fbs[1].tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, renderer.screen_width, renderer.screen_height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// render tails to fb0
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, priv->fbs[0].fbo);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		auto &prog = priv->tail_prog;
		prog.use();
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
	}

	// render horizontal blur from tex0 to fb1
	{
		glBindTexture(GL_TEXTURE_2D, priv->fbs[0].tex);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, priv->fbs[1].fbo);
		auto &prog = priv->horiz_blur_prog;
		prog.use();
		prog.uniform("tex", 0);
		prog.uniform("screen_height_inv", 1.0f/renderer.screen_height);
		prog.enable_attrib_array("vertex");
		prog.attrib_ptr("vertex", &fsquad_vertices[0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(fsquad_vertices));
		GL::Program::clear();
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// render vertical blur from tex1 to screen
	{
		glBindTexture(GL_TEXTURE_2D, priv->fbs[1].tex);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		auto &prog = priv->vert_blur_prog;
		prog.use();
		prog.uniform("tex", 0);
		prog.uniform("screen_height_inv", 1.0f/renderer.screen_height);
		prog.enable_attrib_array("vertex");
		prog.attrib_ptr("vertex", &fsquad_vertices[0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(fsquad_vertices));
		GL::Program::clear();
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

static bool tail_segment_expired(const TailSegment &ts) {
	return ts.b.color.a <= 0.0f;
}

void TailBatch::tick(const Game &game) {
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
