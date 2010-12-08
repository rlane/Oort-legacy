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
#include "util.h"

static int scn_team(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	const char *filename = luaL_checkstring(L, 2);
	long color = luaL_checklong(L, 3);
	team_create(name, filename, color);
	return 0;
}

static int scn_ship(lua_State *L)
{
	const char *ship_class_name = luaL_checkstring(L, 1);
	const char *team_name = luaL_checkstring(L, 2);
	double x = luaL_checknumber(L, 3);
	double y = luaL_checknumber(L, 4);
	const char *orders = luaL_optstring(L, 5, "");

	struct team *team = team_lookup(team_name);
	if (!team) return luaL_argerror(L, 2, "invalid team");

	struct ship *s = ship_create(team->filename, ship_class_name, team, C(x,y), C(0,0), orders);
	if (!s) return luaL_error(L, "ship creation failed");
	return 0;
}

int load_scenario(const char *filename, int num_teams, char **teams)
{
	lua_State *L = luaL_newstate();

	luaL_openlibs(L);
	lua_register(L, "team", scn_team);
	lua_register(L, "ship", scn_ship);

	lua_pushstring(L, data_dir);
	lua_setglobal(L, "data_dir");

	lua_pushnumber(L, num_teams);
	lua_setglobal(L, "N");

	lua_newtable(L);
	int i;
	for (i = 0; i < num_teams; i++) {
		lua_pushnumber(L, i);
		lua_pushstring(L, teams[i]);
		lua_settable(L, -3);
	}
	lua_setglobal(L, "AI");

	if (luaL_dofile(L, filename)) {
		fprintf(stderr, "Failed to load scenario %s: %s\n", filename, lua_tostring(L, -1));
		lua_close(L);
		return -1;
	}

	lua_close(L);
	return 0;
}
