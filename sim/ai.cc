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

LuaAI::LuaAI(Ship &ship, std::string filename, std::string code)
	: AI(ship)
{
	G = luaL_newstate();
	if (!G) {
		throw std::runtime_error("Failed to create Lua state");
	}

	luaL_openlibs(G);

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
		throw std::runtime_error("AI error"); // XXX message, traceback
	}
}

}
