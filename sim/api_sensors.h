#ifndef _API_SENSORS_H
#define _API_SENSORS_H

void ud_sensor_contact_register(lua_State *L);
int api_sensor_contacts(lua_State *L);
int api_sensor_contact(lua_State *L);

#endif
