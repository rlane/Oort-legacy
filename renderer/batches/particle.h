#ifndef OORT_RENDERER_BATCHES_PARTICLE_H_
#define OORT_RENDERER_BATCHES_PARTICLE_H_

#include <boost/random/mersenne_twister.hpp>
#include "renderer/batch.h"
#include "glm/glm.hpp"
#include "gl/program.h"
#include "gl/texture.h"

namespace Oort {

enum class ParticleType {
	HIT = 0,
	PLASMA = 1,
	ENGINE = 2,
	EXPLOSION = 3,
};

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
	boost::random::mt19937 prng;

	ParticleBatch(Renderer &Renderer);
	virtual void render();
	virtual void tick();

private:
	void create_texture();
	void shower(ParticleType type, glm::vec2 p0, glm::vec2 v0, glm::vec2 v, float s_max, float life_min, float life_max, int count);
};

}
}

#endif
