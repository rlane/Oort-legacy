#include <glib.h>

#ifndef BULLET_H
#define BULLET_H

// keep in sync with ships.lua
enum bullet_type {
	BULLET_SLUG = 1,
	BULLET_PLASMA,
};

struct bullet {
	struct physics *physics;
	struct _RISCTeam *team;
	double ttl;
	int dead;
	int type;
};

extern GList *all_bullets;

struct bullet *bullet_create();
void bullet_purge(void);
void bullet_shutdown(void);
void bullet_tick(double t);

#endif
