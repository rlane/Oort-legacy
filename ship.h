#include <complex.h>
#include <lua.h>
#include <lauxlib.h>
#include <glib.h>

#ifndef SHIP_H
#define SHIP_H

#define TAIL_SEGMENTS 16
#define TAIL_TICKS 4
#define MAX_DEBUG_LINES 32

struct ship {
	guint32 api_id;
	const struct _RISCShipClass *class;
	struct _RISCTeam *team;
	struct _RISCPhysics *physics;
	double energy, hull;
	lua_State *lua, *global_lua;
	struct {
		lua_Alloc allocator;
		void *allocator_ud;
		int cur, limit;
	} mem;
	GRand *prng;
	int dead, ai_dead;
	Vec2 tail[TAIL_SEGMENTS];
	int tail_head;
	int last_shot_tick;
	GQueue *mq;
	guint64 line_start_time;
	char line_info[256];
	struct {
		int num_lines;
		struct {
			Vec2 a, b;
		} lines[MAX_DEBUG_LINES];
	} debug;
	struct {
		const struct gfx_class *class;
		double angle;
	} gfx;
};

extern GList *all_ships;

typedef void (*RISCOnShipCreated)(struct ship *s);
extern RISCOnShipCreated gfx_ship_create_cb;

struct ship *ship_create(const char *filename, const char *class_name, struct _RISCTeam *team, Vec2 p, Vec2 v, const char *orders, int seed);
void ship_purge();
void ship_shutdown();
void ship_tick(double t);
double ship_get_energy(struct ship *s);
struct ship *lua_ship(lua_State *L);

#endif
