#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <glib.h>
#include <stdio.h>

#include "particle.h"

struct particle particles[MAX_PARTICLES];
static unsigned short pclock;

void particle_create(enum particle_type type,
		                 Vec2 p, Vec2 v,
										 float initial_time,
										 float lifetime)
{
	particles[pclock++] = (struct particle){ p, v, initial_time, lifetime, type };
}

void particle_shower(enum particle_type type, float initial_time,
		                 Vec2 p0, Vec2 v0, Vec2 v, double s_max,
										 unsigned short life_ticks_min, unsigned short life_ticks_max,
										 unsigned short count)
{

	int i;
	for (i = 0; i < count; i++) {
		double a = g_random_double() * G_PI * 2;
		double s = g_random_double() * s_max;
		double fdp = g_random_double();
		Vec2 dp = vec2_scale(v, fdp);
		int life_ticks = (life_ticks_min == life_ticks_max) ? life_ticks_min : g_random_int_range(life_ticks_min,life_ticks_max);
		float lifetime = life_ticks/32.0;
		Vec2 dv = vec2(cos(a)*s, sin(a)*s);
		particle_create(type, vec2_add(vec2_add(p0,dp),dv), vec2_add(vec2_add(v0,v),dv), initial_time, lifetime);
	}
}
