// Copyright 2011 Rich Lane
#ifndef OORT_RENDERER_RENDERER_H_
#define OORT_RENDERER_RENDERER_H_

#include <memory>
#include <boost/scoped_ptr.hpp>
#include "sim/game.h"
#include "gl/program.h"
#include "gl/texture.h"

namespace Oort {

namespace RendererBatches {
	class Batch;
}

struct Text {
	int x, y;
	std::string str;

#if 0
	Text(int &_x, int &_y, std::string &_str)
		: x(_x), y(_y), str(std::move(_str)) {}
#endif
};

class Renderer {
public:
	Game &game;
	glm::mat4 p_matrix;
	std::vector<Text> texts;
	int screen_width, screen_height;
	float aspect_ratio;

	Renderer(Game &game);
	void reshape(int screen_width, int screen_height);
	void render(float view_radius, glm::vec2 view_center);
	void tick();
	void text(int x, int y, const std::string &str);
	glm::vec2 pixel2screen(glm::vec2 p);

private:
	std::vector<RendererBatches::Batch*> batches;
};

}

#endif
