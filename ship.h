#include <complex.h>
#include <lua.h>
#include <lauxlib.h>
#include <glib.h>

#ifndef SHIP_H
#define SHIP_H

struct ship_class {
	double energy_max, energy_rate;
	double r;
};

#define TAIL_SEGMENTS 4

struct ship {
	const struct ship_class *class;
	struct physics *physics;
	double energy;
	lua_State *lua;
	int ai_dead;
	complex double tail[TAIL_SEGMENTS];
	int tail_head;
};

extern const struct ship_class fighter, mothership;
extern GList *all_ships;

struct ship *ship_create(char *filename, const struct ship_class *class);
void ship_destroy(struct ship *s);
void ship_tick(double t);

#endif
