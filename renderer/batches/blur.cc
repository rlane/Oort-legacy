#include "renderer/batches/blur.h"

#include <memory>
#include <list>
#include "glm/gtc/matrix_transform.hpp"
#include "gl/check.h"
#include "gl/program.h"
#include "gl/texture.h"
#include "common/resources.h"

using glm::vec2;
using glm::vec4;
using std::make_shared;
using std::shared_ptr;

namespace Oort {

namespace RendererBatches {

struct BlurPriv {
	GL::Program horiz_prog;
	GL::Program vert_prog;

	BlurPriv()
		: horiz_prog(GL::Program::from_resources("identity", "gaussian_horiz")),
		  vert_prog(GL::Program::from_resources("identity", "gaussian_vert"))
	{
	}
};

BlurBatch::BlurBatch(Renderer &renderer)
	: Batch(renderer),
	  priv(make_shared<BlurPriv>())
{
}

void BlurBatch::render(float time_delta) {
}

void BlurBatch::tick(const Game &game) {
}

}
}
