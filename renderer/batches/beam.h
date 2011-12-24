#ifndef OORT_RENDERER_BATCHES_BEAM_H_
#define OORT_RENDERER_BATCHES_BEAM_H_

#include "renderer/batch.h"
#include "gl/program.h"

namespace Oort {
namespace RendererBatches {

class BeamBatch : public Batch {
public:
	GL::Program prog;

	BeamBatch(Renderer &Renderer);
	virtual void render();
};

}
}

#endif
