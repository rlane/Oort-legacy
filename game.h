#include <glib.h>
#include <stdio.h>
#include "vector.h"

#ifndef GAME_H
#define GAME_H

struct bullet_hit {
	struct ship *s;
	struct bullet *b;
	vec2 cp;
	double e;
};

int game_init(int seed, const char *scenario, char **teams, int num_teams);
void game_tick(double tick_length);
void game_purge();
void game_shutdown();
struct team *game_check_victory(void);

extern int ticks;
extern double current_time;
extern GList *bullet_hits;
extern GRand *prng;
extern FILE *trace_file;

#endif
