#include <glib.h>
#include "vector.h"

#ifndef RISC_H
#define RISC_H

struct bullet_hit {
	struct ship *s;
	struct bullet *b;
	vec2 cp;
};

int game_init();
void game_tick(double tick_length);
void game_purge();
void game_shutdown();
struct team *game_check_victory(void);

extern int ticks;
extern double current_time;
extern GList *bullet_hits;
extern GRand *prng;

#endif
