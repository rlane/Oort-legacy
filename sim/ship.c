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
#include "api_team.h"
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

static int api_thrust(lua_State *L)
{
	RISCShip *s = lua_ship(L);
	double a = luaL_checknumber(L, 1);
	double f = luaL_checknumber(L, 2);
	s->physics->thrust = vec2_scale(vec2(cos(a), sin(a)), f * s->physics->m);
	return 0;
}

static int api_yield(lua_State *L)
{
	if (LW_VERBOSE) fprintf(stderr, "api_yield\n");
	return lua_yield(L, 0);
}

static int api_position(lua_State *L)
{
	RISCShip *s = lua_ship(L);
	lua_pushnumber(L, s->physics->p.x);
	lua_pushnumber(L, s->physics->p.y);
	return 2;
}

static int api_velocity(lua_State *L)
{
	RISCShip *s = lua_ship(L);
	lua_pushnumber(L, s->physics->v.x);
	lua_pushnumber(L, s->physics->v.y);
	return 2;
}

static int api_create_bullet(lua_State *L)
{
	RISCShip *s = lua_ship(L);

	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double vx = luaL_checknumber(L, 3);
	double vy = luaL_checknumber(L, 4);
	double m = luaL_checknumber(L, 5);
	double ttl = luaL_checknumber(L, 6);
	int type = luaL_checkint(L, 7);

	risc_bullet_create(s->team, vec2(x,y), vec2(vx,vy), 1.0/32, m, ttl, type);

	return 0;
}

static int api_class(lua_State *L)
{
	RISCShip *s = lua_ship(L);
	lua_pushstring(L, s->class->name);
	return 1;
}

static int api_time(lua_State *L)
{
	lua_pushnumber(L, ticks/32.0);
	return 1;
}

static int api_random(lua_State *L)
{
	RISCShip *s = lua_ship(L);
	int n = lua_gettop(L);

	if (n == 0) {
		lua_settop(L, 0);
		lua_pushnumber(L, g_rand_double(s->prng));
	} else if (n == 1 || n == 2) {
		guint32 begin, end;
		if (n == 1) {
			begin = 1;
			end = luaL_checklong(L, 1);
		} else {
			begin = luaL_checklong(L, 1);
			end = luaL_checklong(L, 2);
		}
		lua_settop(L, 0);
		if (begin < end) {
			lua_pushnumber(L, g_rand_int_range(s->prng, begin, end));
		} else if (begin == end) {
			lua_pushnumber(L, begin);
		} else {
			return luaL_error(L, "end must be >= begin");
		}
	} else {
		return luaL_error(L, "too many arguments");
	}
	
	return 1;
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

static int api_spawn(lua_State *L)
{
	RISCShip *s = lua_ship(L);
	const char *class_name = luaL_checkstring(L, 1);
	const char *orders = luaL_checkstring(L, 2);
	const char *filename = s->team->filename;

	RISCShip *child = ship_create(filename, class_name, s->team,
			                             s->physics->p, s->physics->v,
																	 orders, g_rand_int(s->prng));
	if (!child) return luaL_error(L, "failed to create ship");
	return 0;
}

static int api_die(lua_State *L)
{
	RISCShip *s = lua_ship(L);
	s->dead = 1;
	return lua_yield(L, 0);
}	

static int api_debug_line(lua_State *L)
{
	double x1,y1,x2,y2;
	RISCShip *s = lua_ship(L);
	if (s->debug.num_lines == RISC_SHIP_MAX_DEBUG_LINES) {
		return 0;
	}
	int i = s->debug.num_lines++;
	x1 = luaL_checknumber(L, 1);
	y1 = luaL_checknumber(L, 2);
	x2 = luaL_checknumber(L, 3);
	y2 = luaL_checknumber(L, 4);
	s->debug.lines[i].a = vec2(x1,y1);
	s->debug.lines[i].b = vec2(x2,y2);
	return 0;
}

static int api_clear_debug_lines(lua_State *L)
{
	RISCShip *s = lua_ship(L);
	s->debug.num_lines = 0;
	return 0;
}

static int api_serialize_id(lua_State *L)
{
	luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
	guint32 id = (guint32)(uintptr_t)lua_touserdata(L, 1);
	char *buf = (char*)&id;
	lua_pushlstring(L, buf, sizeof(id));
	return 1;
}

static int api_deserialize_id(lua_State *L)
{
	const char *buf = luaL_checkstring(L, 1);
	const guint32 *ibuf = (void*)buf;
	guint32 id = *ibuf;
	lua_pushlightuserdata(L, (void*)(uintptr_t)id);
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
	lua_register(G, "sys_thrust", api_thrust);
	lua_register(G, "sys_yield", api_yield);
	lua_register(G, "sys_position", api_position);
	lua_register(G, "sys_velocity", api_velocity);
	lua_register(G, "sys_create_bullet", api_create_bullet);
	lua_register(G, "sys_sensor_contacts", api_sensor_contacts);
	lua_register(G, "sys_sensor_contact", api_sensor_contact);
	lua_register(G, "sys_team", api_team);
	lua_register(G, "sys_class", api_class);
	lua_register(G, "sys_time", api_time);
	lua_register(G, "sys_random", api_random);
	lua_register(G, "sys_send", api_send);
	lua_register(G, "sys_recv", api_recv);
	lua_register(G, "sys_spawn", api_spawn);
	lua_register(G, "sys_die", api_die);
	lua_register(G, "sys_debug_line", api_debug_line);
	lua_register(G, "sys_clear_debug_lines", api_clear_debug_lines);
	lua_register(G, "sys_serialize_id", api_serialize_id);
	lua_register(G, "sys_deserialize_id", api_deserialize_id);

	lua_registry_set(G, RKEY_SHIP, s);

	ud_sensor_contact_register(G);
	ud_team_register(G);

	lua_pushstring(G, orders);
	lua_setglobal(G, "orders");

	lua_pushstring(G, data_dir);
	lua_setglobal(G, "data_dir");

	if (luaL_dofile(G, data_path("runtime.lua"))) {
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
