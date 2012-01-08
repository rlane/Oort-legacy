#ifndef OORT_RENDERER_BATCHES_CLEAR_H_
#define OORT_RENDERER_BATCHES_CLEAR_H_

#include "renderer/batch.h"

namespace Oort {

namespace RendererBatches {

class ClearBatch : public Batch {
public:
	ClearBatch(Renderer &Renderer);
	virtual void render(float time_delta);
};

}
}

#endif
