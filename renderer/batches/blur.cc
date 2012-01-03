#include "renderer/batches/blur.h"

#include <memory>
#include <list>
#include "glm/gtc/matrix_transform.hpp"
#include "gl/check.h"
#include "gl/program.h"
#include "gl/texture.h"
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

struct BlurPriv {
	GL::Program horiz_prog;
	GL::Program vert_prog;
	FramebufferTexture fbs[2];

	BlurPriv()
		: horiz_prog(GL::Program::from_resources("gaussian", "gaussian_horiz")),
		  vert_prog(GL::Program::from_resources("gaussian", "gaussian_vert"))
	{
	}
};

BlurBatch::BlurBatch(Renderer &renderer)
	: Batch(renderer),
	  priv(make_shared<BlurPriv>())
{
}

void BlurBatch::render(float time_delta) {
	std::vector<vec2> vertices = { vec2(1,0), vec2(0,0), vec2(1,1), vec2(0,1) };

	glBindTexture(GL_TEXTURE_2D, priv->fbs[1].tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, renderer.screen_width, renderer.screen_height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);

	// copy from screen to tex0, then render from tex0 to fb1
	glBindTexture(GL_TEXTURE_2D, priv->fbs[0].tex);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 0, 0, renderer.screen_width, renderer.screen_height, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, priv->fbs[1].fbo);
	{
		auto &prog = priv->horiz_prog;
		prog.use();
		prog.uniform("tex", 0);
		prog.uniform("screen_width_inv", 1.0f/renderer.screen_width);
		prog.enable_attrib_array("vertex");
		prog.attrib_ptr("vertex", &vertices[0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(vertices));
		GL::Program::clear();
	}

	// render from tex1 to screen
	glBindTexture(GL_TEXTURE_2D, priv->fbs[1].tex);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	{
		auto &prog = priv->vert_prog;
		prog.use();
		prog.uniform("tex", 0);
		prog.uniform("screen_height_inv", 1.0f/renderer.screen_height);
		prog.enable_attrib_array("vertex");
		prog.attrib_ptr("vertex", &vertices[0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(vertices));
		GL::Program::clear();
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void BlurBatch::tick(const Game &game) {
}

}
}
