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
	int count_for_victory;
};

#define TAIL_SEGMENTS 16
#define TAIL_TICKS 4
#define MAX_DEBUG_LINES 32

struct ship {
	guint32 api_id;
	const struct ship_class *class;
	struct team *team;
	struct physics *physics;
	double energy, hull;
	lua_State *lua, *global_lua;
	struct {
		lua_Alloc allocator;
		void *allocator_ud;
		int cur, limit;
	} mem;
	GRand *prng;
	int dead, ai_dead;
	complex double tail[TAIL_SEGMENTS];
	int tail_head;
	int last_shot_tick;
	GQueue *mq;
	guint64 line_start_time;
	char line_info[256];
	struct {
		int num_lines;
		struct {
			vec2 a, b;
		} lines[MAX_DEBUG_LINES];
	} debug;
	struct {
		const struct gfx_class *class;
		double angle;
	} gfx;
};

extern const struct ship_class fighter, mothership;
extern GList *all_ships;
extern GHashTable *ship_classes;
extern void (*gfx_ship_create_cb)(struct ship *s);

struct ship *ship_create(const char *filename, const char *class_name, struct team *team, vec2 p, vec2 v, const char *orders, int seed);
void ship_purge();
void ship_shutdown();
void ship_tick(double t);
int load_ship_classes(const char *filename);
double ship_get_energy(struct ship *s);
struct ship *lua_ship(lua_State *L);

#endif
