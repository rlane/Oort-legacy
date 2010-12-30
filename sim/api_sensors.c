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

#include <glib-object.h>
#include "risc.h"
#include "ship.h"

struct sensor_query {
	const RISCTeam *my_team;
	int enemy;
	const RISCShipClass *class;
	double distance_lt, distance_gt;
	double hull_lt, hull_gt;
	unsigned int limit;
	Vec2 origin;
};

void parse_sensor_query(lua_State *L, struct sensor_query *query, int idx)
{
	const char *str;
	double num;
	int i;
	RISCShip *s = lua_ship(L);

	query->my_team = s->team;
	query->origin = s->physics->p;

	lua_pushstring(L, "enemy");
	lua_rawget(L, idx);
	if ((lua_isboolean(L, -1))) {
		query->enemy = lua_toboolean(L, -1);
	} else {
		query->enemy = -1;
	}
	lua_pop(L, 1);

	lua_pushstring(L, "class");
	lua_rawget(L, idx);
	if ((str = lua_tostring(L, -1))) {
		query->class = risc_shipclass_lookup(str);
	} else {
		query->class = NULL;
	}
	lua_pop(L, 1);

	lua_pushstring(L, "distance_lt");
	lua_rawget(L, idx);
	if ((num = lua_tonumber(L, -1))) {
		query->distance_lt = num;
	} else {
		query->distance_lt = NAN;
	}
	lua_pop(L, 1);

	lua_pushstring(L, "distance_gt");
	lua_rawget(L, idx);
	if ((num = lua_tonumber(L, -1))) {
		query->distance_gt = num;
	} else {
		query->distance_gt = NAN;
	}
	lua_pop(L, 1);

	lua_pushstring(L, "hull_lt");
	lua_rawget(L, idx);
	if ((num = lua_tonumber(L, -1))) {
		query->hull_lt = num;
	} else {
		query->hull_lt = NAN;
	}
	lua_pop(L, 1);

	lua_pushstring(L, "hull_gt");
	lua_rawget(L, idx);
	if ((num = lua_tonumber(L, -1))) {
		query->hull_gt = num;
	} else {
		query->hull_gt = NAN;
	}
	lua_pop(L, 1);

	lua_pushstring(L, "limit");
	lua_rawget(L, idx);
	if ((i = lua_tointeger(L, -1))) {
		query->limit = i;
	} else {
		query->limit = -1;
	}
	lua_pop(L, 1);
}

int match_sensor_query(const struct sensor_query *query, const RISCShip *s)
{
	if (query->enemy == 0 && query->my_team != s->team) return 0;
	if (query->enemy == 1 && query->my_team == s->team) return 0;
	if (query->class && query->class != s->class) return 0;
	double distance = vec2_distance(query->origin, s->physics->p);
	if (!isnan(query->distance_lt) && query->distance_lt <= distance) return 0;
	if (!isnan(query->distance_gt) && query->distance_gt >= distance) return 0;
	if (!isnan(query->hull_lt) && query->hull_lt <= s->hull) return 0;
	if (!isnan(query->hull_gt) && query->hull_gt >= s->hull) return 0;
	return 1;
}

int api_sensor_contacts(lua_State *L)
{
	struct sensor_query query;
	luaL_checktype(L, 1, LUA_TTABLE);
	parse_sensor_query(L, &query, 1);

	lua_pushlightuserdata(L, (void*)RISC_SHIP_SENSOR_CONTACT_MAGIC);
	lua_rawget(L, LUA_REGISTRYINDEX);
	int metatable_index = lua_gettop(L);

	lua_createtable(L, g_list_length(all_ships), 0);
	int i;
	GList *e;
	for (e = g_list_first(all_ships), i = 1;
			 e && i <= query.limit;
			 e = g_list_next(e)) {
		RISCShip *s = e->data;
		if (match_sensor_query(&query, s)) {
			risc_ship_sensor_contact_create(L, s, metatable_index);
			lua_rawseti(L, -2, i);
			i++;
		}
	}

	return 1;
}

int api_sensor_contact(lua_State *L)
{
	GList *e;
	size_t n;
	const char *id = lua_tolstring(L, 1, &n);
	if (!id || n != sizeof(guint32)) {
		return luaL_error(L, "invalid contact id");
	}
	lua_pop(L, 1);
	for (e = g_list_first(all_ships); e; e = g_list_next(e)) {
		RISCShip *s = e->data;
		if (!memcmp(id, &s->api_id, sizeof(s->api_id))) {
			lua_pushlightuserdata(L, (void*)RISC_SHIP_SENSOR_CONTACT_MAGIC);
			lua_rawget(L, LUA_REGISTRYINDEX);
			risc_ship_sensor_contact_create(L, s, lua_gettop(L));
			return 1;
		}
	}
	return 0;
}
