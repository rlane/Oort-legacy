#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "game.h"
#include "vector.h"
#include "ship.h"
#include "physics.h"
#include "team.h"

static int scn_team(lua_State *L)
{
	const char *name = lua_tolstring(L, 1, NULL);
	long color = lua_tointeger(L, 2);
	team_create(name, color);
	return 0;
}

static int scn_ship(lua_State *L)
{
	const char *ship_class_name = luaL_checkstring(L, 1);
	const char *filename = luaL_checkstring(L, 2);
	const char *team_name = luaL_checkstring(L, 3);
	double x = luaL_checknumber(L, 4);
	double y = luaL_checknumber(L, 5);
	const char *orders = luaL_optstring(L, 6, "");
	struct team *team = team_lookup(team_name);
	struct ship *s = ship_create(filename, ship_class_name, orders);
	s->physics->p = C(x,y);
	s->physics->v = 0;
	s->team = team;
	return 0;
}

int load_scenario(const char *filename)
{
	lua_State *L;

	L = luaL_newstate();
	luaL_openlibs(L);

	lua_register(L, "team", scn_team);
	lua_register(L, "ship", scn_ship);

	if (luaL_dofile(L, filename)) {
		fprintf(stderr, "Failed to loading scenario %s: %s\n", filename, lua_tostring(L, -1));
		lua_close(L);
		return -1;
	}

	lua_close(L);
	return 0;
}
