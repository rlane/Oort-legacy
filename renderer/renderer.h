// Copyright 2011 Rich Lane
#ifndef OORT_RENDERER_RENDERER_H_
#define OORT_RENDERER_RENDERER_H_

#include <memory>
#include <boost/scoped_ptr.hpp>
#include "sim/game.h"
#include "gl/program.h"
#include "gl/buffer.h"
#include "gl/texture.h"

namespace Oort {

namespace RendererBatches {
	class Batch;
}

class Renderer {
public:
	std::shared_ptr<Game> game;
	glm::mat4 p_matrix;

	Renderer(std::shared_ptr<Game> game);
	void reshape(int screen_width, int screen_height);
	void render(float view_radius, glm::vec2 view_center);
	void tick();
	void text(int x, int y, const std::string &str);

private:
	boost::scoped_ptr<GL::Program> beam_prog;
	boost::scoped_ptr<GL::Program> text_prog;
	GL::Texture font_tex;
	int screen_width, screen_height;
	float aspect_ratio;
	std::vector<RendererBatches::Batch*> batches;

	void load_font();
	void render_beams();
};

}

#endif
