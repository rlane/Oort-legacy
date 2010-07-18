#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <glib.h>

#include "bullet.h"
#include "physics.h"

GList *all_bullets = NULL;

struct bullet *bullet_create(void)
{
	struct bullet *b = g_slice_new(struct bullet);
	b->physics = physics_create();
	b->physics->r = 1.0/32;
	b->ttl = 1;
	all_bullets = g_list_append(all_bullets, b);
	return b;
}

void bullet_destroy(struct bullet *b)
{
	all_bullets = g_list_remove(all_bullets, b);
	physics_destroy(b->physics);
	g_slice_free(struct bullet, b);
}

void bullet_tick_one(struct bullet *b, double *ta)
{
	double t = *ta;
	b->ttl -= t;
	if (b->ttl <= 0) {
		bullet_destroy(b);
	}
}

void bullet_tick(double t)
{
	g_list_foreach(all_bullets, (GFunc)bullet_tick_one, &t);
}

