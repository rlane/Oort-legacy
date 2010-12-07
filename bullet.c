#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <glib.h>

#include "bullet.h"
#include "physics.h"

GList *all_bullets = NULL;
static GList *new_bullets = NULL;
static GStaticMutex new_bullets_lock = G_STATIC_MUTEX_INIT;

struct bullet *bullet_create(void)
{
	struct bullet *b = g_slice_new(struct bullet);
	b->physics = physics_create();
	b->physics->r = 1.0/32;
	b->physics->m = 0.1;
	b->ttl = 1;
	b->dead = 0;
	b->type = 0;
	g_static_mutex_lock(&new_bullets_lock);
	new_bullets = g_list_append(new_bullets, b);
	g_static_mutex_unlock(&new_bullets_lock);
	return b;
}

void bullet_destroy(struct bullet *b)
{
	all_bullets = g_list_remove(all_bullets, b);
	physics_destroy(b->physics);
	g_slice_free(struct bullet, b);
}

void bullet_purge(void)
{
	GList *e, *e2;
	for (e = g_list_first(all_bullets); e; e = e2) {
		struct bullet *b = e->data;
		e2 = g_list_next(e);
		if (b->dead) {
			bullet_destroy(b);
		}
	}
}

void bullet_promote(void)
{
	all_bullets = g_list_concat(all_bullets, new_bullets);
	new_bullets = NULL;
}

void bullet_shutdown(void)
{
	bullet_promote();
	g_list_foreach(all_bullets, (GFunc)bullet_destroy, NULL);
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
	bullet_promote();
	g_list_foreach(all_bullets, (GFunc)bullet_tick_one, &t);
}

