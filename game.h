#include <glib.h>
#include "vector.h"

#ifndef RISC_H
#define RISC_H

struct team {
	int color;
	char *name;
};

struct bullet_hit {
	struct ship *s;
	struct bullet *b;
	vec2 cp;
};

void game_tick(double tick_length);
void game_purge();

extern int ticks;
extern GList *bullet_hits;

#endif
