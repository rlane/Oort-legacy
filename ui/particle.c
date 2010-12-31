#include <stdlib.h>
#include <unistd.h>
#include <complex.h>
#include <math.h>
#include <glib.h>
#include <stdio.h>

#include "particle.h"

struct particle particles[MAX_PARTICLES];
static unsigned short pclock;

void particle_create(enum particle_type type,
		                 Vec2 p, Vec2 v,
										 unsigned short lifetime)
{
	particles[pclock++] = (struct particle){ p, v, lifetime, type };
}

void particle_shower(enum particle_type type,
		                 Vec2 p0, Vec2 v0, Vec2 v, double s_max,
										 unsigned short life_min, unsigned short life_max,
										 unsigned short count)
{

	int i;
	for (i = 0; i < count; i++) {
		double a = g_random_double() * G_PI * 2;
		double s = g_random_double() * s_max;
		double fdp = g_random_double();
		Vec2 dp = vec2_scale(v, fdp);
		int life = (life_min == life_max) ? life_min : g_random_int_range(life_min,life_max);
		Vec2 dv = vec2(cos(a)*s, sin(a)*s);
		particle_create(type, vec2_add(p0,dp), vec2_add(vec2_add(v0,v),dv), life);
	}
}

void particle_tick(void)
{
	int i;
	for (i = 0; i < MAX_PARTICLES; i++) {
		struct particle *c = &particles[i];
		if (c->ticks_left == 0) continue;
		c->ticks_left--;
		c->p = vec2_add(c->p, c->v);
	}
}
