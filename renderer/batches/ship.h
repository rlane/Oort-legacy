#ifndef OORT_RENDERER_BATCHES_SHIP_H_
#define OORT_RENDERER_BATCHES_SHIP_H_

#include "renderer/batch.h"
#include "gl/program.h"

namespace Oort {
namespace RendererBatches {

class ShipBatch : public Batch {
public:
	GL::Program prog;

	ShipBatch(Renderer &Renderer);
	virtual void render();
};

}
}

#endif
