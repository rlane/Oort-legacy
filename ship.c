#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <lua.h>
#include <lauxlib.h>

#include "ship.h"
#include "physics.h"

#define LW_VERBOSE 0

const struct ship_class fighter = {
	.energy_max = 1.0,
	.energy_rate = 0.1,
	.r = 1.0,
};

char RKEY_SHIP[1];

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

static int api_fire(lua_State *L)
{
	struct ship *s = lua_ship(L);
	return 0;
}

static lua_State *ai_create(char *filename)
{
	lua_State *G, *L;

	G = luaL_newstate();
	luaL_openlibs(G);
	lua_register(G, "thrust", api_thrust);
	lua_register(G, "yield", api_yield);
	lua_register(G, "position", api_position);
	lua_register(G, "fire", api_fire);

	L = lua_newthread(G);

	if (luaL_loadfile(L, filename)) {
		fprintf(stderr, "Couldn't load file %s: %s\n", filename, lua_tostring(L, -1));
		// XXX free
		return NULL;
	}

	return L;
}

static void count_hook(lua_State *L, lua_Debug *ar)
{
	if (LW_VERBOSE) fprintf(stderr, "count hook fired\n");
	lua_yield(L, 0);
}

int ship_run(struct ship *s, int len)
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

struct ship *ship_create(char *filename)
{
	struct ship *s = calloc(1, sizeof(*s));
	if (!s) return NULL;

	s->lua = ai_create(filename);
	if (!s->lua) {
		fprintf(stderr, "failed to create AI\n");
		return NULL;
	}

	lua_registry_set(s->lua, RKEY_SHIP, s);

	s->physics = physics_create();

	s->ai_dead = 0;
	s->class = &fighter;

	int i;
	for (i = 0; i < TAIL_SEGMENTS; i++) {
		s->tail[i] = NAN;
	}

	s->tail_head = 0;

	return s;
}
