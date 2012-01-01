#ifndef OORT_RENDERER_BATCHES_BEAM_H_
#define OORT_RENDERER_BATCHES_BEAM_H_

#include "renderer/batch.h"
#include "gl/program.h"

namespace Oort {
namespace RendererBatches {

struct BeamPriv;

class BeamBatch : public Batch {
public:
	BeamBatch(Renderer &Renderer);
	virtual void tick();
	virtual void render();

private:
	std::shared_ptr<BeamPriv> priv;
};

}
}

#endif
