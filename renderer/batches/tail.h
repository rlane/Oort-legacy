#ifndef OORT_RENDERER_BATCHES_TAIL_H_
#define OORT_RENDERER_BATCHES_TAIL_H_

#include "renderer/batch.h"
#include "gl/program.h"

namespace Oort {
namespace RendererBatches {

class TailPriv;

class TailBatch : public Batch {
public:
	TailBatch(Renderer &Renderer);
	virtual void render(float time_delta);
	virtual void tick(const Game &game);
private:
	std::shared_ptr<TailPriv> priv;
};

}
}

#endif
