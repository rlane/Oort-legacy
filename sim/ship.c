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

#include <glib.h>
#include <glib-object.h>
#include "risc.h"
#include "ship.h"
#include "api_sensors.h"
#include "util.h"

#define LW_VERBOSE 0

char RKEY_SHIP[1];

RISCOnShipCreated gfx_ship_create_cb;
static const int ai_mem_limit = 1<<20;
FILE *trace_file = NULL;

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

RISCShip *lua_ship(lua_State *L)
{
	return lua_registry_get(L, RKEY_SHIP);
}

static void *ai_allocator(RISCShip *s, void *ptr, size_t osize, size_t nsize)
{
	s->mem.cur += (nsize - osize);
	if (nsize > osize && s->mem.cur > s->mem.limit) {
		return NULL;
	}
	return s->mem.allocator(s->mem.allocator_ud, ptr, osize, nsize);
}

int risc_ship_create_ai(RISCShip *s, guint8 *orders, size_t orders_len)
{
	lua_State *G, *L;

	G = luaL_newstate();
	s->mem.cur = 0;
	s->mem.limit = ai_mem_limit;
	s->mem.allocator = lua_getallocf(G, &s->mem.allocator_ud);
	lua_setallocf(G, (lua_Alloc)ai_allocator, s);
	luaL_openlibs(G);
	lua_register(G, "sys_thrust", risc_ship_api_thrust);
	lua_register(G, "sys_yield", risc_ship_api_yield);
	lua_register(G, "sys_position", risc_ship_api_position);
	lua_register(G, "sys_velocity", risc_ship_api_velocity);
	lua_register(G, "sys_create_bullet", risc_ship_api_create_bullet);
	lua_register(G, "sys_sensor_contacts", api_sensor_contacts);
	lua_register(G, "sys_sensor_contact", api_sensor_contact);
	lua_register(G, "sys_random", risc_ship_api_random);
	lua_register(G, "sys_send", risc_ship_api_send);
	lua_register(G, "sys_recv", risc_ship_api_recv);
	lua_register(G, "sys_spawn", risc_ship_api_spawn);
	lua_register(G, "sys_die", risc_ship_api_die);
	lua_register(G, "sys_debug_line", risc_ship_api_debug_line);
	lua_register(G, "sys_clear_debug_lines", risc_ship_api_clear_debug_lines);

	lua_registry_set(G, RKEY_SHIP, s);

	ud_sensor_contact_register(G);

	lua_pushstring(G, s->class->name);
	lua_setglobal(G, "class");

	lua_pushstring(G, s->team->name);
	lua_setglobal(G, "team");

	lua_pushlstring(G, orders, orders_len);
	lua_setglobal(G, "orders");

	char *data_dir = g_file_get_path(risc_paths_resource_dir);
	lua_pushstring(G, data_dir);
	lua_setglobal(G, "data_dir");
	g_free(data_dir);

	char *runtime_filename = risc_data_path("runtime.lua");
	if (luaL_dofile(G, runtime_filename)) {
		g_warning("Failed to load runtime: %s", lua_tostring(G, -1));
		lua_close(G);
		g_free(runtime_filename);
		return 1;
	}
	g_free(runtime_filename);

	L = lua_newthread(G);

	lua_getglobal(L, "sandbox");

	if (luaL_loadfile(L, s->team->filename)) {
		g_warning("Couldn't load file %s: %s", s->team->filename, lua_tostring(L, -1));
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
#ifdef CLOCK_THREAD_CPUTIME_ID
	struct timespec ts;
	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts)) {
		perror("glock_gettime");
		abort();
	}
	return ts.tv_nsec + ts.tv_sec*(1000*1000*1000);
#else
	return 0;
#endif
}

void debug_hook(lua_State *L, lua_Debug *a)
{
	if (a->event == LUA_HOOKCOUNT) {
		lua_getglobal(L, "debug_count_hook");
		lua_call(L, 0, 0);
		lua_yield(L, 0);
	} else if (a->event == LUA_HOOKLINE) {
		RISCShip *s = lua_ship(L);
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

double ship_get_energy(RISCShip *s)
{
	lua_getglobal(s->global_lua, "energy");
	lua_call(s->global_lua, 0, 1);
	double e = lua_tonumber(s->global_lua, -1);
	lua_pop(s->global_lua, 1);
	return e;
}
