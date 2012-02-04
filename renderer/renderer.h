// Copyright 2011 Rich Lane
#ifndef OORT_RENDERER_RENDERER_H_
#define OORT_RENDERER_RENDERER_H_

#include <memory>
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "renderer/perf.h"

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
	glm::mat4 p_matrix;
	std::vector<Text> texts;
	int screen_width, screen_height;
	float aspect_ratio;
	float view_scale;
	PerfHistogram render_perf;
	PerfHistogram snapshot_perf;
	bool benchmark;

	Renderer();
	void reshape(int screen_width, int screen_height);
	void render(float view_radius, glm::vec2 view_center, float time_delta);
	void snapshot(const Game &game);
	void text(int x, int y, const std::string &str);
	glm::vec2 pixel2screen(glm::vec2 p);
	void dump_perf();

private:
	std::vector<std::shared_ptr<RendererBatches::Batch>> batches;
	template <typename T> void add_batch();
};

}

#endif
