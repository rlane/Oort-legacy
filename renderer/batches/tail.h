#ifndef OORT_RENDERER_BATCHES_TAIL_H_
#define OORT_RENDERER_BATCHES_TAIL_H_

#include "renderer/batch.h"
#include "gl/program.h"

namespace Oort {
namespace RendererBatches {

struct TailVertex {
	glm::vec2 p;
	glm::vec4 color;
};

struct TailSegment {
	TailVertex a, b;
};

class TailBatch : public Batch {
public:
	GL::Program prog;
	std::vector<TailSegment> tail_segments;

	TailBatch(Renderer &Renderer);
	virtual void render();
	virtual void tick();
};

}
}

#endif
