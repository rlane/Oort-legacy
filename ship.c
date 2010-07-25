#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "game.h"
#include "physics.h"
#include "bullet.h"
#include "ship.h"
#include "team.h"

#define LW_VERBOSE 0

char RKEY_SHIP[1];

GList *all_ships = NULL;
static GHashTable *ship_classes = NULL;
static int next_ship_id = 1;

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

static struct ship *lua_ship(lua_State *L)
{
	return lua_registry_get(L, RKEY_SHIP);
}

static int api_thrust(lua_State *L)
{
	struct ship *s = lua_ship(L);
	double a = luaL_optnumber(L, 1, 0);
	double f = luaL_optnumber(L, 2, 0);
	s->physics->thrust = f * (cos(a) + sin(a)*I);
	//printf("thrust x=%g y=%g\n", creal(s->physics->thrust), cimag(s->physics->thrust));
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
	lua_pushnumber(L, creal(s->physics->p));
	lua_pushnumber(L, cimag(s->physics->p));
	return 2;
}

static int api_velocity(lua_State *L)
{
	struct ship *s = lua_ship(L);
	lua_pushnumber(L, creal(s->physics->v));
	lua_pushnumber(L, cimag(s->physics->v));
	return 2;
}

static int api_create_bullet(lua_State *L)
{
	struct ship *s = lua_ship(L);

	double x = lua_tonumber(L, 1);
	double y = lua_tonumber(L, 2);
	double vx = lua_tonumber(L, 3);
	double vy = lua_tonumber(L, 4);
	double m = lua_tonumber(L, 5);
	double ttl = lua_tonumber(L, 6);

	struct bullet *b = bullet_create();
	b->team = s->team;
	b->physics->p = C(x,y);
	b->physics->v = C(vx,vy);
	b->physics->m = m;
	b->ttl = ttl;

	return 0;
}

static int api_sensor_contacts(lua_State *L)
{
	int i = 1;
	lua_newtable(L);
	GList *e;
	for (e = g_list_first(all_ships); e; e = g_list_next(e)) {
		struct ship *s = e->data;
		lua_pushnumber(L, i++);
		lua_newtable(L);

		lua_pushstring(L, "team");
		lua_pushstring(L, s->team->name);
		lua_settable(L, -3);

		lua_pushstring(L, "x"); // index 3
		lua_pushnumber(L, creal(s->physics->p)); // index 4
		lua_settable(L, -3);

		lua_pushstring(L, "y"); // index 3
		lua_pushnumber(L, cimag(s->physics->p)); // index 4
		lua_settable(L, -3);

		lua_pushstring(L, "vx");
		lua_pushnumber(L, creal(s->physics->v));
		lua_settable(L, -3);

		lua_pushstring(L, "vy");
		lua_pushnumber(L, cimag(s->physics->v));
		lua_settable(L, -3);

		lua_settable(L, -3);
	}
	return 1;
}

static int api_team(lua_State *L)
{
	struct ship *s = lua_ship(L);
	lua_pushstring(L, s->team->name);
	return 1;
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
			end = lua_tointeger(L, -1);
		} else {
			begin = lua_tointeger(L, -2);
			end = lua_tointeger(L, -1);
		}
		lua_settop(L, 0);
		lua_pushnumber(L, g_rand_int_range(s->prng, begin, end));
	} else {
	//	lua_pushstring(L, "too many arguments");
		//lua_error(L);
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
	const char *ldata = lua_tolstring(L, -1, &len);

	char *data = malloc(len);
	if (!data) abort();
	memcpy(data, ldata, len);

	struct msg *msg = g_slice_new(struct msg);
	msg->refcount = 1;
	msg->len = len;
	msg->data = data;

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
		
	return 0;	
}

static int api_recv(lua_State *L)
{
	struct ship *s = lua_ship(L);

	struct msg *msg = g_queue_pop_head(s->mq);
	if (!msg) return 0;
	lua_pushlstring(L, msg->data, msg->len);

	if (--msg->refcount == 0) {
		free(msg->data);
		g_slice_free(struct msg, msg);
	}

	return 1;
}

static lua_State *ai_create(const char *filename, struct ship *s)
{
	lua_State *G, *L;

	G = luaL_newstate();
	luaL_openlibs(G);
	lua_register(G, "sys_thrust", api_thrust);
	lua_register(G, "sys_yield", api_yield);
	lua_register(G, "sys_position", api_position);
	lua_register(G, "sys_velocity", api_velocity);
	lua_register(G, "sys_create_bullet", api_create_bullet);
	lua_register(G, "sys_sensor_contacts", api_sensor_contacts);
	lua_register(G, "sys_team", api_team);
	lua_register(G, "sys_class", api_class);
	lua_register(G, "sys_time", api_time);
	lua_register(G, "sys_random", api_random);
	lua_register(G, "sys_send", api_send);
	lua_register(G, "sys_recv", api_recv);

	lua_registry_set(G, RKEY_SHIP, s);

	lua_pushnumber(G, s->id);
	lua_setglobal(G, "ship_id");

	if (luaL_dofile(G, "runtime.lua")) {
		fprintf(stderr, "Failed to load runtime: %s\n", lua_tostring(G, -1));
		return NULL;
	}

	L = lua_newthread(G);

	lua_getglobal(L, "sandbox");

	if (luaL_loadfile(L, filename)) {
		fprintf(stderr, "Couldn't load file %s: %s\n", filename, lua_tostring(L, -1));
		// XXX free
		return NULL;
	}

	lua_call(L, 1, 1);

	return L;
}

static void count_hook(lua_State *L, lua_Debug *a)
{
	lua_getglobal(L, "debug_count_hook");
	lua_call(L, 0, 0);
	lua_yield(L, 0);
}

int ship_ai_run(struct ship *s, int len)
{
	int result;
	double returned;
	lua_State *L = s->lua;

	lua_sethook(L, count_hook, LUA_MASKCOUNT, len);

	result = lua_resume(L, 0);
	if (result == LUA_YIELD) {
		if (LW_VERBOSE) fprintf(stderr, "script yielded\n");
		return 1;
	} else if (result == 0) {
		/* Get the returned value at the top of the stack (index -1) */
		returned = lua_tonumber(L, -1);
		if (LW_VERBOSE) fprintf(stderr, "script returned: %.0f\n", returned);
		lua_pop(L, 1);	/* Take the returned value out of the stack */
		return 0;
	} else {
		fprintf(stderr, "script error: %s\n", lua_tostring(L, -1));
		return 0;
	}
}

void ship_tick_one(struct ship *s, void *unused)
{
	if (ticks % TAIL_TICKS == 0) {
		s->tail[s->tail_head++] = s->physics->p;
		if (s->tail_head == TAIL_SEGMENTS) s->tail_head = 0;
	}

	if (!s->ai_dead) {
		int ret = ship_ai_run(s, 5000);
		if (!ret) s->ai_dead = 1;
	}
}

void ship_tick(double t)
{
	g_list_foreach(all_ships, (GFunc)ship_tick_one, NULL);
}

struct ship *ship_create(const char *filename, const char *class_name)
{
	struct ship *s = g_slice_new0(struct ship);

	s->id = next_ship_id++;

	s->class = g_hash_table_lookup(ship_classes, class_name);
	if (!s->class) {
		fprintf(stderr, "class '%s' not found\n", class_name);
		// XXX free
		return NULL;
	}

	s->lua = ai_create(filename, s);
	if (!s->lua) {
		fprintf(stderr, "failed to create AI\n");
		return NULL;
	}

	s->physics = physics_create();
	s->physics->r = s->class->radius;

	s->dead = 0;
	s->ai_dead = 0;

	s->hull = s->class->hull;

	int i;
	for (i = 0; i < TAIL_SEGMENTS; i++) {
		s->tail[i] = NAN;
	}

	s->tail_head = 0;

	s->prng = g_rand_new_with_seed(g_random_int());
	s->mq = g_queue_new();

	all_ships = g_list_append(all_ships, s);

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

static double lua_getfield_double(lua_State *L, int index, const char *key)
{
	lua_getfield(L, -1, key);
	double v = lua_tonumber(L, -1);
	printf(" %s = %g\n", key, v);
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
		fprintf(stderr, "Fail to load ships from %s: 'ships' table not defined\n", filename);
		lua_close(L);
		return -1;
	}

	ship_classes = g_hash_table_new(g_str_hash, g_str_equal);

	lua_pushnil(L);
	while (lua_next(L, 1) != 0) {
		char *name = g_strdup(lua_tolstring(L, -2, NULL));
		printf("loading ship %s\n", name);
		struct ship_class *c = g_slice_new(struct ship_class);
		c->name = name;
		c->radius = lua_getfield_double(L, -1, "radius");
		c->hull = lua_getfield_double(L, -1, "hull");
		g_hash_table_insert(ship_classes, name, c);
		lua_pop(L, 1);
	}

	lua_close(L);
	return 0;
}
