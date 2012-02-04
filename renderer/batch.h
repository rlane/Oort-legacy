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
	PerfHistogram render_perf;
	PerfHistogram snapshot_perf;

	Batch(Renderer &renderer)
		: renderer(renderer) {}

	virtual void snapshot(const Game &game) {};
	virtual void render(float time) {};
};

}
}

#endif
