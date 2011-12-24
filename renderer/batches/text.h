#ifndef OORT_RENDERER_BATCHES_TEXT_H_
#define OORT_RENDERER_BATCHES_TEXT_H_

#include "renderer/batch.h"
#include "gl/program.h"

namespace Oort {
namespace RendererBatches {

class TextBatch : public Batch {
public:
	GL::Program prog;
	GL::Texture font_tex;

	TextBatch(Renderer &Renderer);
	virtual void render();
};

}
}

#endif
