#include "renderer/batches/clear.h"

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

ClearBatch::ClearBatch(Renderer &renderer)
	: Batch(renderer)
{
}

void ClearBatch::render(float time_delta) {
	glClearColor(0.0f, 0.0f, 0.03f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

}
}
