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
#include "api_team.h"

char UKEY_TEAM[1];

struct ud_team {
	const void *magic;
	const struct team *team;
};

void ud_team_new(lua_State *L, const struct team *team)
{
	struct ud_team *u = lua_newuserdata(L, sizeof(*u));
	u->magic = UKEY_TEAM;
	u->team = team;
	lua_pushlightuserdata(L, UKEY_TEAM);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}

static struct ud_team *ud_team_cast(lua_State *L, int index)
{
	struct ud_team *u = lua_touserdata(L, index);
	if (u->magic != UKEY_TEAM) {
		u = NULL;
	}
	luaL_argcheck(L, u != NULL, 1, "team expected");
	return u;
}

static int ud_team_tostring(lua_State *L)
{
	struct ud_team *u = ud_team_cast(L, -1);
	lua_pushstring(L, u->team->name);
	return 1;
}

static int ud_team_eq(lua_State *L)
{
	struct ud_team *a = ud_team_cast(L, 1);
	struct ud_team *b = ud_team_cast(L, 2);
	if (!a || !b) luaL_error(L, "bad args");
	lua_pushboolean(L, a->team == b->team);
	return 1;
}

void ud_team_register(lua_State *L)
{
	lua_pushlightuserdata(L, UKEY_TEAM);
	lua_createtable(L, 0, 2);

	lua_pushstring(L, "__tostring");
	lua_pushcfunction(L, ud_team_tostring);
	lua_settable(L, -3);

	lua_pushstring(L, "__eq");
	lua_pushcfunction(L, ud_team_eq);
	lua_settable(L, -3);

	lua_settable(L, LUA_REGISTRYINDEX);
}

int api_team(lua_State *L)
{
	struct ship *s = lua_ship(L);
	ud_team_new(L, s->team);
	return 1;
}
