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

	L = lua_newthread(G);
	if (luaL_loadbuffer(L, code.c_str(), code.length(), filename.c_str())) {
		throw std::runtime_error("Failed to load Lua AI"); // XXX message
	}
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
	api_push_vector(L, ship.get_position());
	return 1;
}

void LuaAI::register_api() {
	lua_register(G, "position", api_position);
}

}
