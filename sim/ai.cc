// Copyright 2011 Rich Lane
#include "sim/ai.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "glm/glm.hpp"
#include "glm/gtx/vector_angle.hpp"
#include <Box2D/Box2D.h>

#include "sim/game.h"
#include "sim/ship.h"
#include "sim/bullet.h"
#include "common/log.h"
#include "common/resources.h"

using glm::vec2;

namespace Oort {

AI::AI(Ship &ship) : ship(ship) {}
void AI::tick() {}

CxxAI::CxxAI(Ship &ship) : AI(ship) {}
void CxxAI::tick() {}

LuaAIFactory::LuaAIFactory(std::string filename, std::string code)
	: AIFactory(),
	  filename(filename),
		code(code)
{
}

std::unique_ptr<AI> LuaAIFactory::instantiate(Ship &ship) {
	return std::unique_ptr<AI>(new LuaAI(ship, filename, code));
}

static void *AI_RKEY = (void*)0xAABBCC02;

LuaAI::LuaAI(Ship &ship, std::string filename, std::string code)
	: AI(ship)
{
	G = luaL_newstate();
	if (!G) {
		throw std::runtime_error("Failed to create Lua state");
	}

	lua_pushlightuserdata(G, AI_RKEY);
	lua_pushlightuserdata(G, this);
	lua_settable(G, LUA_REGISTRYINDEX);

	luaL_openlibs(G);
	register_api();

	std::vector<std::string> libs = { "lib", "strict", "vector" };
	BOOST_FOREACH(auto &lib, libs) {
		std::string filename = lib + ".lua";
		auto lib_code = load_resource(filename);
		luaL_loadbuffer(G, lib_code.c_str(), lib_code.length(), filename.c_str());
		lua_setglobal(G, lib.c_str());
	}

	auto runtime_code = load_resource("runtime.lua");
	luaL_loadbuffer(G, runtime_code.c_str(), runtime_code.length(), "runtime.lua");
	lua_call(G, 0, 0);

	L = lua_newthread(G);

	lua_getglobal(L, "sandbox");

	if (luaL_loadbuffer(L, code.c_str(), code.length(), filename.c_str())) {
		throw std::runtime_error("Failed to load Lua AI"); // XXX message
	}

	lua_call(L, 1, 1);
}

LuaAI::~LuaAI() {
	if (G) {
		lua_close(G);
	}
}

void LuaAI::tick() {
	auto result = lua_resume(L, 0);
	if (result == 0) {
		throw std::runtime_error("AI exited");
	} else if (result == LUA_YIELD) {
	} else {
		std::string msg(lua_tostring(L, -1));
		throw std::runtime_error("AI error" + msg); // XXX traceback
	}
}

static LuaAI &lua_ai(lua_State *L) {
	lua_pushlightuserdata(L, AI_RKEY);
	lua_gettable(L, LUA_REGISTRYINDEX);
	void *v = lua_touserdata(L, -1);
	lua_pop(L, 1);
	return *(LuaAI*)v;
}

void api_push_vector(lua_State *L, vec2 v) {
	lua_createtable(L, 2, 0);

	lua_pushnumber(L, v.x);
	lua_rawseti(L, -2, 1);

	lua_pushnumber(L, v.y);
	lua_rawseti(L, -2, 2);
}

int api_position(lua_State *L) {
	auto &ship = lua_ai(L).ship;
	auto p = ship.get_position();
	lua_pushnumber(L, p.x);
	lua_pushnumber(L, p.y);
	return 2;
}

int api_velocity(lua_State *L) {
	auto &ship = lua_ai(L).ship;
	auto v = ship.get_velocity();
	lua_pushnumber(L, v.x);
	lua_pushnumber(L, v.y);
	return 2;
}

int api_heading(lua_State *L) {
	auto &ship = lua_ai(L).ship;
	lua_pushnumber(L, ship.get_heading());
	return 1;
}

int api_angular_velocity(lua_State *L) {
	auto &ship = lua_ai(L).ship;
	lua_pushnumber(L, ship.get_angular_velocity());
	return 1;
}

int api_acc_main(lua_State *L) {
	auto &ship = lua_ai(L).ship;
	float a = luaL_checknumber(L, 1);
	ship.acc_main(a);
	return 0;
}

int api_acc_lateral(lua_State *L) {
	auto &ship = lua_ai(L).ship;
	float a = luaL_checknumber(L, 1);
	ship.acc_lateral(a);
	return 0;
}

int api_acc_angular(lua_State *L) {
	auto &ship = lua_ai(L).ship;
	float a = luaL_checknumber(L, 1);
	ship.acc_angular(a);
	return 0;
}

int api_fire_gun(lua_State *L) {
	auto &ship = lua_ai(L).ship;
	int idx = luaL_checkinteger(L, 1);
	float a = luaL_checknumber(L, 2);
	ship.fire_gun(idx, a);
	return 0;
}

void LuaAI::register_api() {
	lua_register(G, "sys_position", api_position);
	lua_register(G, "sys_velocity", api_velocity);
	lua_register(G, "sys_heading", api_heading);
	lua_register(G, "sys_angular_velocity", api_angular_velocity);
	lua_register(G, "sys_thrust_main", api_acc_main);
	lua_register(G, "sys_thrust_lateral", api_acc_lateral);
	lua_register(G, "sys_thrust_angular", api_acc_angular);
	lua_register(G, "sys_fire_gun", api_fire_gun);
}

}
