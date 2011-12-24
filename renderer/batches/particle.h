#ifndef OORT_RENDERER_BATCHES_PARTICLE_H_
#define OORT_RENDERER_BATCHES_PARTICLE_H_

#include "renderer/batch.h"
#include "glm/glm.hpp"
#include "gl/program.h"
#include "gl/texture.h"

namespace Oort {
namespace RendererBatches {

struct Particle {
	glm::vec2 initial_position;
	glm::vec2 velocity;
	float initial_time;
	float lifetime;
	float type;
	float padding;
};

class ParticleBatch : public Batch {
public:
	GL::Program prog;
	std::vector<Particle> particles;
	GL::Texture tex;

	ParticleBatch(Renderer &Renderer);
	virtual void render();
	virtual void tick();

private:
	void create_texture();
};

}
}

#endif
