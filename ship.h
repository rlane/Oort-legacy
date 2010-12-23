#include <complex.h>
#include <lua.h>
#include <lauxlib.h>
#include <glib.h>

#ifndef SHIP_H
#define SHIP_H

struct _RISCShip;
struct _RISCTeam;

extern GList *all_ships;

typedef void (*RISCOnShipCreated)(struct _RISCShip *s);
extern RISCOnShipCreated gfx_ship_create_cb;

struct _RISCShip *ship_create(const char *filename, const char *class_name, struct _RISCTeam *team, Vec2 p, Vec2 v, const char *orders, int seed);
void ship_purge();
void ship_shutdown();
void ship_tick(double t);
double ship_get_energy(struct _RISCShip *s);
struct _RISCShip *lua_ship(lua_State *L);

#endif
