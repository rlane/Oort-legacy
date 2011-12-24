// Copyright 2011 Rich Lane
#ifndef OORT_RENDERER_RENDERER_H_
#define OORT_RENDERER_RENDERER_H_

#include <memory>
#include <string>
#include <vector>
#include "glm/glm.hpp"

namespace Oort {

class Game;

namespace RendererBatches {
	class Batch;
}

struct Text {
	int x, y;
	std::string str;
};

class Renderer {
public:
	Game &game;
	glm::mat4 p_matrix;
	std::vector<Text> texts;
	int screen_width, screen_height;
	float aspect_ratio;
	float view_scale;

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
