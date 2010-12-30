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
