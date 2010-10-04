#include <stdlib.h>
#include <unistd.h>
#include <complex.h>
#include <math.h>
#include <glib.h>

#define GL_GLEXT_PROTOTYPES
#include <SDL.h>
#include <SDL_opengl.h>

#include "particle.h"

struct particle particles[MAX_PARTICLES];
static unsigned short pclock;

void particle_create(enum particle_type type,
		                 complex float p, complex float v,
										 unsigned short lifetime)
{
	particles[pclock++] = (struct particle){ p, v, lifetime, type, 0 };
}

void particle_shower(enum particle_type type,
		                 complex float p, complex float v, float s_max,
										 unsigned short life_min, unsigned short life_max,
										 unsigned short count)
{

	int i;
	for (i = 0; i < count; i++) {
		float a = (float)(g_random_double() * G_PI * 2);
		float s = (float)g_random_double()*s_max;
		float fdp = (float)g_random_double();
		complex float dp = fdp*v;
		int life = g_random_int_range(life_min,life_max);
		complex float dv = cosf(a) * s + I * sinf(a) * s;
		particle_create(type, dp+p, dv+v, life);
	}
}

void particle_tick(void)
{
	int i;
	for (i = 0; i < MAX_PARTICLES; i++) {
		struct particle *c = &particles[i];
		if (c->ticks_left == 0) continue;
		c->ticks_left--;
		c->p += c->v;
	}
}
