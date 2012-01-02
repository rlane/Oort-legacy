#ifndef OORT_RENDERER_BATCHES_SHIP_H_
#define OORT_RENDERER_BATCHES_SHIP_H_

#include "renderer/batch.h"
#include "gl/program.h"

namespace Oort {
namespace RendererBatches {

class ShipPriv;

class ShipBatch : public Batch {
public:
	ShipBatch(Renderer &Renderer);
	virtual void render();
	virtual void tick(const Game &game);

private:
	std::shared_ptr<ShipPriv> priv;
};

}
}

#endif
