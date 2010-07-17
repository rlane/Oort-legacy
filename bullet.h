#include <glib.h>

#ifndef BULLET_H
#define BULLET_H

struct bullet {
	struct physics *physics;
	double ttl;
};

extern GList *all_bullets;

struct bullet *bullet_create();
void bullet_destroy(struct bullet *b);
void bullet_tick(double t);

#endif
