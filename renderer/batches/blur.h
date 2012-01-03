#ifndef OORT_RENDERER_BATCHES_BLUR_H_
#define OORT_RENDERER_BATCHES_BLUR_H_

#include "renderer/batch.h"

namespace Oort {

namespace RendererBatches {

class BlurPriv;

class BlurBatch : public Batch {
public:
	BlurBatch(Renderer &Renderer);
	virtual void render(float time_delta);
	virtual void tick(const Game &game);

private:
	std::shared_ptr<BlurPriv> priv;
};

}
}

#endif
