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

#include "game.h"
#include "physics.h"
#include "bullet.h"
#include "ship.h"
#include "team.h"
#include "task.h"
#include "api_sensors.h"
#include "api_team.h"
#include "util.h"

#define LW_VERBOSE 0

char RKEY_SHIP[1];

GList *all_ships = NULL;
GHashTable *ship_classes;
void (*gfx_ship_create_cb)(struct ship *s);
static GList *new_ships = NULL;
static GStaticMutex new_ships_lock = G_STATIC_MUTEX_INIT;
static GStaticMutex radio_lock = G_STATIC_MUTEX_INIT;
static const int ai_mem_limit = 1<<20;

void ship_destroy(struct ship *s);

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

struct ship *lua_ship(lua_State *L)
{
	return lua_registry_get(L, RKEY_SHIP);
}

static int api_thrust(lua_State *L)
{
	struct ship *s = lua_ship(L);
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
	struct ship *s = lua_ship(L);
	lua_pushnumber(L, s->physics->p.x);
	lua_pushnumber(L, s->physics->p.y);
	return 2;
}

static int api_velocity(lua_State *L)
{
	struct ship *s = lua_ship(L);
	lua_pushnumber(L, s->physics->v.x);
	lua_pushnumber(L, s->physics->v.y);
	return 2;
}

static int api_create_bullet(lua_State *L)
{
	struct ship *s = lua_ship(L);

	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double vx = luaL_checknumber(L, 3);
	double vy = luaL_checknumber(L, 4);
	double m = luaL_checknumber(L, 5);
	double ttl = luaL_checknumber(L, 6);
	int type = luaL_checkint(L, 7);

	struct bullet *b = bullet_create();
	if (!b) return luaL_error(L, "bullet creation failed");

	b->team = s->team;
	b->physics->p = vec2(x,y);
	b->physics->v = vec2(vx,vy);
	b->physics->m = m;
	b->ttl = ttl;
	b->type = type;

	return 0;
}

static int api_class(lua_State *L)
{
	struct ship *s = lua_ship(L);
	lua_pushstring(L, s->class->name);
	return 1;
}

static int api_time(lua_State *L)
{
	lua_pushnumber(L, current_time);
	return 1;
}

static int api_random(lua_State *L)
{
	struct ship *s = lua_ship(L);
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
	struct ship *s = lua_ship(L);
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
		struct ship *s2 = e->data;
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
	struct ship *s = lua_ship(L);

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
	struct ship *s = lua_ship(L);
	const char *class_name = luaL_checkstring(L, 1);
	const char *orders = luaL_checkstring(L, 2);
	const char *filename = s->team->filename;

	struct ship *child = ship_create(filename, class_name, s->team,
			                             s->physics->p, s->physics->v, orders);
	if (!child) return luaL_error(L, "failed to create ship");
	return 0;
}

static int api_die(lua_State *L)
{
	struct ship *s = lua_ship(L);
	s->dead = 1;
	return lua_yield(L, 0);
}	

static int api_debug_line(lua_State *L)
{
	double x1,y1,x2,y2;
	struct ship *s = lua_ship(L);
	if (s->debug.num_lines == MAX_DEBUG_LINES) {
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
	struct ship *s = lua_ship(L);
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

static void *ai_allocator(struct ship *s, void *ptr, size_t osize, size_t nsize)
{
	s->mem.cur += (nsize - osize);
	if (nsize > osize && s->mem.cur > s->mem.limit) {
		return NULL;
	}
	return s->mem.allocator(s->mem.allocator_ud, ptr, osize, nsize);
}

static int ai_create(const char *filename, struct ship *s, const char *orders)
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
#ifdef WINDOWS
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

static void debug_hook(lua_State *L, lua_Debug *a)
{
	if (a->event == LUA_HOOKCOUNT) {
		lua_getglobal(L, "debug_count_hook");
		lua_call(L, 0, 0);
		lua_yield(L, 0);
	} else if (a->event == LUA_HOOKLINE) {
		struct ship *s = lua_ship(L);
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

static int ship_ai_run(struct ship *s, int len)
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

void ship_tick_one(struct ship *s)
{
	if (ticks % TAIL_TICKS == 0) {
		s->tail[s->tail_head++] = s->physics->p;
		if (s->tail_head == TAIL_SEGMENTS) s->tail_head = 0;
	}

	if (!s->ai_dead) {
		int ret = ship_ai_run(s, 10000);
		if (!ret) s->ai_dead = 1;
	}

	if (!s->ai_dead) {
		lua_sethook(s->global_lua, NULL, 0, 0);
		lua_getglobal(s->global_lua, "tick_hook");
		lua_call(s->global_lua, 0, 0);
	}
}

double ship_get_energy(struct ship *s)
{
	lua_getglobal(s->global_lua, "energy");
	lua_call(s->global_lua, 0, 1);
	double e = lua_tonumber(s->global_lua, -1);
	lua_pop(s->global_lua, 1);
	return e;
}

void ship_tick(double t)
{
	all_ships = g_list_concat(all_ships, new_ships);
	new_ships = NULL;
	GList *e;
	for (e = g_list_first(all_ships); e; e = g_list_next(e)) {
		task((task_func)ship_tick_one, e->data, NULL);
	}
	task_wait();
}

struct ship *ship_create(const char *filename, const char *class_name, struct team *team,
		                     Vec2 p, Vec2 v, const char *orders)
{
	struct ship *s = g_slice_new0(struct ship);

	s->team = team;

	s->class = g_hash_table_lookup(ship_classes, class_name);
	if (!s->class) {
		fprintf(stderr, "class '%s' not found\n", class_name);
		g_slice_free(struct ship, s);
		return NULL;
	}

	s->physics = physics_create();
	s->physics->r = s->class->radius;
	s->physics->p = p;
	s->physics->v = v;

	s->dead = 0;
	s->ai_dead = 0;

	s->hull = s->class->hull;

	int i;
	for (i = 0; i < TAIL_SEGMENTS; i++) {
		s->tail[i] = vec2(NAN,NAN);
	}

	s->tail_head = 0;

	s->prng = g_rand_new_with_seed(g_rand_int(prng));
	s->mq = g_queue_new();
	s->api_id = g_rand_int(prng);

	g_static_mutex_lock(&new_ships_lock);
	new_ships = g_list_append(new_ships, s);
	g_static_mutex_unlock(&new_ships_lock);

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

void ship_destroy(struct ship *s)
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

	physics_destroy(s->physics);
	lua_close(s->lua);
	g_rand_free(s->prng);
	g_slice_free(struct ship, s);
}

void ship_purge(void)
{
	GList *e, *e2;
	for (e = g_list_first(all_ships); e; e = e2) {
		struct ship *s = e->data;
		e2 = g_list_next(e);
		if (s->dead) {
			ship_destroy(s);
		}
	}
}

static int free_ship_class(char *name, struct ship_class *class)
{
	free(name);
	g_slice_free(struct ship_class, class);
	return TRUE;
}

void ship_shutdown(void)
{
	g_list_foreach(all_ships, (GFunc)ship_destroy, NULL);

	if (ship_classes) {
		g_hash_table_foreach_remove(ship_classes, (GHRFunc)free_ship_class, NULL);
		g_hash_table_destroy(ship_classes);
		ship_classes = NULL;
	}
}

static double lua_getfield_double(lua_State *L, int index, const char *key)
{
	lua_getfield(L, -1, key);
	double v = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return v;
}

int load_ship_classes(const char *filename)
{
	lua_State *L;

	L = luaL_newstate();
	//luaL_openlibs(L);

	if (luaL_dofile(L, filename)) {
		fprintf(stderr, "Failed to load ships from %s: %s\n", filename, lua_tostring(L, -1));
		lua_close(L);
		return -1;
	}

	lua_getglobal(L, "ships");

	if (lua_isnil(L, 1)) {
		fprintf(stderr, "Failed to load ships from %s: 'ships' table not defined\n", filename);
		lua_close(L);
		return -1;
	}

	ship_classes = g_hash_table_new(g_str_hash, g_str_equal);

	lua_pushnil(L);
	while (lua_next(L, 1) != 0) {
		char *name = g_strdup(lua_tolstring(L, -2, NULL));
		struct ship_class *c = g_slice_new(struct ship_class);
		c->name = name;
		c->radius = lua_getfield_double(L, -1, "radius");
		c->hull = lua_getfield_double(L, -1, "hull");
		lua_getfield(L, -1, "count_for_victory");
		c->count_for_victory = lua_toboolean(L, -1);
		lua_pop(L, 1);
		g_hash_table_insert(ship_classes, name, c);
		lua_pop(L, 1);
	}

	lua_close(L);
	return 0;
}
