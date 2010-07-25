#include <glib.h>

#ifndef BULLET_H
#define BULLET_H

struct bullet {
	struct physics *physics;
	struct team *team;
	double ttl;
	int dead;
};

extern GList *all_bullets;

struct bullet *bullet_create();
void bullet_purge(void);
void bullet_shutdown(void);
void bullet_tick(double t);

#endif
