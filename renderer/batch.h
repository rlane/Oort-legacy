#ifndef OORT_RENDERER_BATCH_H_
#define OORT_RENDERER_BATCH_H_

#include <memory>
#include "renderer/renderer.h"
#include "renderer/perf.h"

namespace Oort {

class Game;

namespace RendererBatches {

class Batch {
public:
	Renderer &renderer;
	Game &game;
	PerfHistogram render_perf;
	PerfHistogram tick_perf;

	Batch(Renderer &renderer)
		: renderer(renderer),
		  game(renderer.game) {}

	virtual void tick() {};
	virtual void render() {};
};

}
}

#endif
