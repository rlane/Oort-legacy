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

#include "risc.h"
#include "physics.h"
#include "bullet.h"
#include "ship.h"
#include "task.h"
#include "api_team.h"

char UKEY_SENSOR_CONTACT[1];

struct sensor_contact {
	void *magic;
	guint32 id;
	const RISCTeam *team;
	const struct ship_class *class;
	Vec2 p, v;
};

struct sensor_query {
	const RISCTeam *my_team;
	int enemy;
	const struct ship_class *class;
	double distance_lt, distance_gt;
	double hull_lt, hull_gt;
	unsigned int limit;
	Vec2 origin;
};

static void ud_sensor_contact_new(lua_State *L, struct ship *s, int metatable_index)
{
	struct sensor_contact *c = lua_newuserdata(L, sizeof(*c));
	c->magic = UKEY_SENSOR_CONTACT;
	c->id = s->api_id;
	c->team = s->team;
	c->class = s->class;
	c->p = s->physics->p;
	c->v = s->physics->v;
	lua_pushvalue(L, metatable_index);
	lua_setmetatable(L, -2);
}

static struct sensor_contact *ud_sensor_contact_cast(lua_State *L, int index)
{
	struct sensor_contact *c = lua_touserdata(L, index);
	if (c->magic != UKEY_SENSOR_CONTACT) {
		c = NULL;
	}
	luaL_argcheck(L, c != NULL, 1, "sensor contact expected");
	return c;
}

static int ud_sensor_contact_id(lua_State *L)
{
	struct sensor_contact *c = ud_sensor_contact_cast(L, 1);
	lua_pushlightuserdata(L, (void*)(uintptr_t)c->id);
	return 1;
}

static int ud_sensor_contact_team(lua_State *L)
{
	struct sensor_contact *c = ud_sensor_contact_cast(L, 1);
	ud_team_new(L, c->team);
	return 1;
}

static int ud_sensor_contact_class(lua_State *L)
{
	struct sensor_contact *c = ud_sensor_contact_cast(L, 1);
	lua_pushstring(L, c->class->name);
	return 1;
}

static int ud_sensor_contact_position(lua_State *L)
{
	struct sensor_contact *c = ud_sensor_contact_cast(L, 1);
	lua_pushnumber(L, c->p.x);
	lua_pushnumber(L, c->p.y);
	return 2;
}

static int ud_sensor_contact_velocity(lua_State *L)
{
	struct sensor_contact *c = ud_sensor_contact_cast(L, 1);
	lua_pushnumber(L, c->v.x);
	lua_pushnumber(L, c->v.y);
	return 2;
}

void ud_sensor_contact_register(lua_State *L)
{
	lua_pushlightuserdata(L, UKEY_SENSOR_CONTACT);
	lua_createtable(L, 0, 1);

	lua_pushstring(L, "__index");
	lua_createtable(L, 0, 5);

	lua_pushstring(L, "id");
	lua_pushcfunction(L, ud_sensor_contact_id);
	lua_settable(L, -3);

	lua_pushstring(L, "team");
	lua_pushcfunction(L, ud_sensor_contact_team);
	lua_settable(L, -3);

	lua_pushstring(L, "class");
	lua_pushcfunction(L, ud_sensor_contact_class);
	lua_settable(L, -3);

	lua_pushstring(L, "position");
	lua_pushcfunction(L, ud_sensor_contact_position);
	lua_settable(L, -3);

	lua_pushstring(L, "velocity");
	lua_pushcfunction(L, ud_sensor_contact_velocity);
	lua_settable(L, -3);

	lua_settable(L, -3);
	lua_settable(L, LUA_REGISTRYINDEX);
}

void parse_sensor_query(lua_State *L, struct sensor_query *query, int idx)
{
	const char *str;
	double num;
	int i;
	struct ship *s = lua_ship(L);

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
		query->class = g_hash_table_lookup(ship_classes, str);
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

int match_sensor_query(const struct sensor_query *query, const struct ship *s)
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

	lua_pushlightuserdata(L, UKEY_SENSOR_CONTACT);
	lua_rawget(L, LUA_REGISTRYINDEX);
	int metatable_index = lua_gettop(L);

	lua_createtable(L, g_list_length(all_ships), 0);
	int i;
	GList *e;
	for (e = g_list_first(all_ships), i = 1;
			 e && i <= query.limit;
			 e = g_list_next(e)) {
		struct ship *s = e->data;
		if (match_sensor_query(&query, s)) {
			ud_sensor_contact_new(L, s, metatable_index);
			lua_rawseti(L, -2, i);
			i++;
		}
	}

	return 1;
}

int api_sensor_contact(lua_State *L)
{
	GList *e;
	luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
	guint32 id = (guint32)(uintptr_t)lua_touserdata(L, 1);
	lua_pop(L, 1);
	for (e = g_list_first(all_ships); e; e = g_list_next(e)) {
		struct ship *s = e->data;
		if (id == s->api_id) {
			lua_pushlightuserdata(L, UKEY_SENSOR_CONTACT);
			lua_rawget(L, LUA_REGISTRYINDEX);
			ud_sensor_contact_new(L, s, lua_gettop(L));
			return 1;
		}
	}
	return 0;
}
