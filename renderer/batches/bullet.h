#ifndef OORT_RENDERER_BATCHES_BULLET_H_
#define OORT_RENDERER_BATCHES_BULLET_H_

#include "renderer/batch.h"
#include "gl/program.h"

namespace Oort {
namespace RendererBatches {

class BulletBatch : public Batch {
public:
	GL::Program prog;

	BulletBatch(Renderer &Renderer);
	virtual void render();
};

}
}

#endif
