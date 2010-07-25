#include <complex.h>
#include <lua.h>
#include <lauxlib.h>
#include <glib.h>

#ifndef SHIP_H
#define SHIP_H

struct ship_class {
	const char *name;
	double radius;
	double hull;
};

#define TAIL_SEGMENTS 16
#define TAIL_TICKS 4

struct ship {
	int id;
	const struct ship_class *class;
	struct team *team;
	struct physics *physics;
	double energy, hull;
	lua_State *lua;
	GRand *prng;
	int dead, ai_dead;
	complex double tail[TAIL_SEGMENTS];
	int tail_head;
	int last_shot_tick;
};

extern const struct ship_class fighter, mothership;
extern GList *all_ships;

struct ship *ship_create(const char *filename, const char *class_name);
void ship_purge();
void ship_tick(double t);
int load_ship_classes(const char *filename);

#endif
