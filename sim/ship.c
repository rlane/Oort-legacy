#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <string.h>
#include <sys/time.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdint.h>

#include <glib.h>
#include <glib-object.h>
#include "risc.h"
#include "ship.h"
#include "api_sensors.h"
#include "util.h"

#define LW_VERBOSE 0

char RKEY_SHIP[1];

RISCOnShipCreated gfx_ship_create_cb;
static GStaticMutex radio_lock = G_STATIC_MUTEX_INIT;
static const int ai_mem_limit = 1<<20;
FILE *trace_file = NULL;

void ship_destroy(RISCShip *s);

static void lua_registry_set(lua_State *L, void *key, void *value)
{
	lua_pushlightuserdata(L, key);
	lua_pushlightuserdata(L, value);
	lua_settable(L, LUA_REGISTRYINDEX);
}

static void *lua_registry_get(lua_State *L, void *key)
{
	lua_pushlightuserdata(L, key);
	lua_gettable(L, LUA_REGISTRYINDEX);
	void *value = lua_touserdata(L, -1);
	lua_pop(L, 1);
	return value;
}

RISCShip *lua_ship(lua_State *L)
{
	return lua_registry_get(L, RKEY_SHIP);
}

struct msg {
	int refcount;
	int len;
	char *data;
};

static int api_send(lua_State *L)
{
	RISCShip *s = lua_ship(L);
	size_t len;
	const char *ldata = luaL_checklstring(L, 1, &len);

	char *data = malloc(len);
	if (!data) abort();
	memcpy(data, ldata, len);

	struct msg *msg = g_slice_new(struct msg);
	msg->refcount = 1;
	msg->len = len;
	msg->data = data;

	g_static_mutex_lock(&radio_lock);

	GList *e;
	for (e = g_list_first(all_ships); e; e = g_list_next(e)) {
		RISCShip *s2 = e->data;
		if (s == s2 || s->team != s2->team) continue;
		msg->refcount++;
		g_queue_push_tail(s2->mq, msg);
	}
	
	if (--msg->refcount == 0) {
		free(msg->data);
		g_slice_free(struct msg, msg);
	}

	g_static_mutex_unlock(&radio_lock);
		
	return 0;	
}

static int api_recv(lua_State *L)
{
	RISCShip *s = lua_ship(L);

	g_static_mutex_lock(&radio_lock);

	struct msg *msg = g_queue_pop_head(s->mq);
	if (!msg) {
		g_static_mutex_unlock(&radio_lock);
		return 0;
	}
	lua_pushlstring(L, msg->data, msg->len);

	if (--msg->refcount == 0) {
		free(msg->data);
		g_slice_free(struct msg, msg);
	}

	g_static_mutex_unlock(&radio_lock);

	return 1;
}

static void *ai_allocator(RISCShip *s, void *ptr, size_t osize, size_t nsize)
{
	s->mem.cur += (nsize - osize);
	if (nsize > osize && s->mem.cur > s->mem.limit) {
		return NULL;
	}
	return s->mem.allocator(s->mem.allocator_ud, ptr, osize, nsize);
}

static int ai_create(const char *filename, RISCShip *s, const char *orders)
{
	lua_State *G, *L;

	G = luaL_newstate();
	s->mem.cur = 0;
	s->mem.limit = ai_mem_limit;
	s->mem.allocator = lua_getallocf(G, &s->mem.allocator_ud);
	lua_setallocf(G, (lua_Alloc)ai_allocator, s);
	luaL_openlibs(G);
	lua_register(G, "sys_thrust", risc_ship_api_thrust);
	lua_register(G, "sys_yield", risc_ship_api_yield);
	lua_register(G, "sys_position", risc_ship_api_position);
	lua_register(G, "sys_velocity", risc_ship_api_velocity);
	lua_register(G, "sys_create_bullet", risc_ship_api_create_bullet);
	lua_register(G, "sys_sensor_contacts", api_sensor_contacts);
	lua_register(G, "sys_sensor_contact", api_sensor_contact);
	lua_register(G, "sys_random", risc_ship_api_random);
	lua_register(G, "sys_send", api_send);
	lua_register(G, "sys_recv", api_recv);
	lua_register(G, "sys_spawn", risc_ship_api_spawn);
	lua_register(G, "sys_die", risc_ship_api_die);
	lua_register(G, "sys_debug_line", risc_ship_api_debug_line);
	lua_register(G, "sys_clear_debug_lines", risc_ship_api_clear_debug_lines);

	lua_registry_set(G, RKEY_SHIP, s);

	ud_sensor_contact_register(G);

	lua_pushstring(G, s->class->name);
	lua_setglobal(G, "class");

	lua_pushstring(G, s->team->name);
	lua_setglobal(G, "team");

	lua_pushstring(G, orders);
	lua_setglobal(G, "orders");

	char *data_dir = g_file_get_path(risc_paths_resource_dir);
	lua_pushstring(G, data_dir);
	lua_setglobal(G, "data_dir");
	g_free(data_dir);

	if (luaL_dofile(G, risc_data_path("runtime.lua"))) {
		fprintf(stderr, "Failed to load runtime: %s\n", lua_tostring(G, -1));
		lua_close(G);
		return 1;
	}

	L = lua_newthread(G);

	lua_getglobal(L, "sandbox");

	if (luaL_loadfile(L, filename)) {
		fprintf(stderr, "Couldn't load file %s: %s\n", filename, lua_tostring(L, -1));
		lua_close(L);
		return 1;
	}

	lua_call(L, 1, 1);

	s->lua = L;
	s->global_lua = G;

	return 0;
}

static guint64 thread_ns(void)
{
#ifdef G_OS_WIN32
	return 0;
#else
	struct timespec ts;
	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts)) {
		perror("glock_gettime");
		abort();
	}
	return ts.tv_nsec + ts.tv_sec*(1000*1000*1000);
#endif
}

void debug_hook(lua_State *L, lua_Debug *a)
{
	if (a->event == LUA_HOOKCOUNT) {
		lua_getglobal(L, "debug_count_hook");
		lua_call(L, 0, 0);
		lua_yield(L, 0);
	} else if (a->event == LUA_HOOKLINE) {
		RISCShip *s = lua_ship(L);
		unsigned long elapsed = thread_ns() - s->line_start_time;
		if (lua_getinfo(L, "nSl", a) == 0) abort();
		if (s->line_start_time != 0) {
			fprintf(trace_file, "%ld\t%u\t%s\n", elapsed, s->api_id, s->line_info);
		}
		snprintf(s->line_info, sizeof(s->line_info), "%s\t%s:%d",
				     a->name, a->short_src, a->currentline);
		s->line_start_time = thread_ns();
	}
}

int ship_ai_run(RISCShip *s, int len)
{
	int result;
	lua_State *L = s->lua;
	int debug_mask = LUA_MASKCOUNT;
	if (trace_file) debug_mask |= LUA_MASKLINE;

	lua_sethook(L, debug_hook, debug_mask, len);

	s->line_start_time = 0;
	result = lua_resume(L, 0);
	if (result == LUA_YIELD) {
		return 1;
	} else if (result == 0) {
		fprintf(stderr, "ship %u terminated\n", s->api_id);
		return 0;
	} else {
		fprintf(stderr, "ship %u error: %s\nbacktrace:\n", s->api_id, lua_tostring(L, -1));
		lua_Debug ar;
		int i;
		for (i = 0; lua_getstack(L, i, &ar); i++) {
			if (!lua_getinfo(L, "nSl", &ar)) abort();
			fprintf(stderr, "  %d: %s %s %s @ %s:%d\n", i, ar.what, ar.namewhat, ar.name, ar.short_src, ar.currentline);
		}
		return 0;
	}
}

double ship_get_energy(RISCShip *s)
{
	lua_getglobal(s->global_lua, "energy");
	lua_call(s->global_lua, 0, 1);
	double e = lua_tonumber(s->global_lua, -1);
	lua_pop(s->global_lua, 1);
	return e;
}

RISCShip *ship_create(const char *filename, const char *class_name, RISCTeam *team,
		                     Vec2 p, Vec2 v, const char *orders, int seed)
{
	RISCShip *s = g_slice_new0(RISCShip);

	s->team = team;

	s->class = risc_shipclass_lookup(class_name);
	if (!s->class) {
		fprintf(stderr, "class '%s' not found\n", class_name);
		g_slice_free(RISCShip, s);
		return NULL;
	}

	// XXX mass
	s->physics = risc_physics_create(p, p, v, vec2(0,0), 0, 0, 1, s->class->radius);

	s->dead = 0;
	s->ai_dead = 0;

	s->hull = s->class->hull;

	int i;
	for (i = 0; i < RISC_SHIP_TAIL_SEGMENTS; i++) {
		s->tail[i] = vec2(NAN,NAN);
	}

	s->tail_head = 0;

	s->prng = g_rand_new_with_seed(seed);
	s->mq = g_queue_new();
	s->api_id = g_rand_int(s->prng);

	g_mutex_lock(new_ships_lock);
	new_ships = g_list_append(new_ships, s);
	g_mutex_unlock(new_ships_lock);

	if (ai_create(filename, s, orders)) {
		fprintf(stderr, "failed to create AI\n");
		ship_destroy(s);
		return NULL;
	}

	if (gfx_ship_create_cb) {
		gfx_ship_create_cb(s);
	}

	return s;
}

void ship_destroy(RISCShip *s)
{
	all_ships = g_list_remove(all_ships, s);

	struct msg *msg;
	while ((msg = g_queue_pop_head(s->mq))) {
		if (--msg->refcount == 0) {
			free(msg->data);
			g_slice_free(struct msg, msg);
		}
	}
	g_queue_free(s->mq);

	risc_physics_free(s->physics);
	lua_close(s->lua);
	g_rand_free(s->prng);
	g_slice_free(RISCShip, s);
}

static double lua_getfield_double(lua_State *L, int index, const char *key)
{
	lua_getfield(L, -1, key);
	double v = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return v;
}
