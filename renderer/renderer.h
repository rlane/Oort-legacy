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

struct TailVertex {
	glm::vec2 p;
	glm::vec4 color;
};

struct TailSegment {
	TailVertex a, b;
};

class Renderer {
public:
	Renderer(std::shared_ptr<Game> game);
	void reshape(int screen_width, int screen_height);
	void render(float view_radius, glm::vec2 view_center);
	void tick();
	void text(int x, int y, const std::string &str);

private:
	std::shared_ptr<Game> game;
	boost::scoped_ptr<GL::Program> ship_prog;
	boost::scoped_ptr<GL::Program> bullet_prog;
	boost::scoped_ptr<GL::Program> beam_prog;
	boost::scoped_ptr<GL::Program> text_prog;
	GL::Texture font_tex;
	int screen_width, screen_height;
	float aspect_ratio;
	std::vector<TailSegment> tail_segments;

	glm::mat4 p_matrix;

	void load_font();
	void render_tails();
	void tick_tails();
	void render_ships();
	void render_bullets();
	void render_beams();
};

}

#endif
