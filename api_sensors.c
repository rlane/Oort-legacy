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

char UKEY_SENSOR_CONTACT[1];

struct sensor_contact {
	void *magic;
	guint32 id;
	const struct team *team;
	const struct ship_class *class;
	vec2 p, v;
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
	lua_pushnumber(L, creal(c->p));
	lua_pushnumber(L, cimag(c->p));
	return 2;
}

static int ud_sensor_contact_velocity(lua_State *L)
{
	struct sensor_contact *c = ud_sensor_contact_cast(L, 1);
	lua_pushnumber(L, creal(c->v));
	lua_pushnumber(L, cimag(c->v));
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

int api_sensor_contacts(lua_State *L)
{
	GList *e;
	lua_pushlightuserdata(L, UKEY_SENSOR_CONTACT);
	lua_rawget(L, LUA_REGISTRYINDEX);
	int metatable_index = lua_gettop(L);
	lua_createtable(L, g_list_length(all_ships), 0);
	int i;
	for (e = g_list_first(all_ships), i = 1; e; e = g_list_next(e), i++) {
		struct ship *s = e->data;
		ud_sensor_contact_new(L, s, metatable_index);
		lua_rawseti(L, -2, i);
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
