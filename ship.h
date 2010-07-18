#include <complex.h>
#include <lua.h>
#include <lauxlib.h>
#include <glib.h>

#ifndef SHIP_H
#define SHIP_H

struct ship_class {
	double energy_max, energy_rate;
	double r;
	double hull_max;
};

#define TAIL_SEGMENTS 4

struct ship {
	const struct ship_class *class;
	struct team *team;
	struct physics *physics;
	double energy, hull;
	lua_State *lua;
	int dead, ai_dead;
	complex double tail[TAIL_SEGMENTS];
	int tail_head;
	int last_shot_tick;
};

extern const struct ship_class fighter, mothership;
extern GList *all_ships;

struct ship *ship_create(char *filename, const struct ship_class *class);
void ship_purge();
void ship_tick(double t);

#endif
