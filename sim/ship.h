#include <lua.h>
#include <lauxlib.h>
#include <glib.h>

#include "vector.h"

#ifndef SHIP_H
#define SHIP_H

struct _RISCShip;
struct _RISCTeam;

extern GList *all_ships;
extern FILE *trace_file;

typedef void (*RISCOnShipCreated)(struct _RISCShip *s);
extern RISCOnShipCreated gfx_ship_create_cb;

struct _RISCShip *ship_create(const char *filename, const char *class_name, struct _RISCTeam *team, Vec2 p, Vec2 v, const char *orders, int seed);
double ship_get_energy(struct _RISCShip *s);
struct _RISCShip *lua_ship(lua_State *L);
void ship_destroy(struct _RISCShip *s);
int ship_ai_run(struct _RISCShip *s, int len);
void debug_hook(lua_State *L, lua_Debug *a);

#endif
