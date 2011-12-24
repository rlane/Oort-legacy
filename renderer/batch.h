#ifndef OORT_RENDERER_BATCH_H_
#define OORT_RENDERER_BATCH_H_

#include <memory>
#include "renderer/renderer.h"

namespace Oort {

class Game;

namespace RendererBatches {

class Batch {
public:
	Renderer &renderer;
	Game &game;

	Batch(Renderer &renderer)
		: renderer(renderer),
		  game(renderer.game) {}

	virtual void tick() {};
	virtual void render() {};
};

}
}

#endif
