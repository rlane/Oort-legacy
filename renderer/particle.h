#include "vec2d.h"

#ifndef PARTICLE_H
#define PARTICLE_H

enum particle_type {
	PARTICLE_HIT,
	PARTICLE_PLASMA,
	PARTICLE_ENGINE,
	PARTICLE_EXPLOSION,
};

struct particle {
	Vec2 p, v;
	float initial_time;
	float lifetime;
	unsigned char type;
};

#define MAX_PARTICLES 65536
extern struct particle particles[MAX_PARTICLES];

void particle_create(enum particle_type type, Vec2 p, Vec2 v, float initial_time, float lifetime);
void particle_shower(enum particle_type type, float initial_time,
		                 Vec2 p0, Vec2 v0, Vec2 v, double s_max,
										 unsigned short life_min, unsigned short life_max,
										 unsigned short count);

static inline struct particle *particle_get(int i) { return particles + i; }

#endif
