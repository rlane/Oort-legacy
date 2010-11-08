#ifndef _API_TEAM_H
#define _API_TEAM_H

void ud_team_register(lua_State *L);
void ud_team_new(lua_State *L, const struct team *team);
int api_team(lua_State *L);

#endif
