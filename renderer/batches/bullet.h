#ifndef OORT_RENDERER_BATCHES_BULLET_H_
#define OORT_RENDERER_BATCHES_BULLET_H_

#include "renderer/batch.h"
#include "gl/program.h"

namespace Oort {
namespace RendererBatches {

class BulletPriv;

class BulletBatch : public Batch {
public:
	BulletBatch(Renderer &Renderer);
	virtual void tick(const Game &game);
	virtual void render(float time_delta);

private:
	std::shared_ptr<BulletPriv> priv;
};

}
}

#endif
