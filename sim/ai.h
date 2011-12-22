// Copyright 2011 Rich Lane
#ifndef OORT_SIM_AI_H_
#define OORT_SIM_AI_H_

#include <string>
#include <memory>

class lua_State;

namespace Oort {

class Ship;
class AI;

class AIFactory {
public:
	AIFactory(std::string name) : name(name) {}
	virtual std::unique_ptr<AI> instantiate(Ship &ship) = 0;

	std::string name;
};

class CxxAIFactory : public AIFactory {
public:
	CxxAIFactory(std::string name) : AIFactory(name) {}
	virtual std::unique_ptr<AI> instantiate(Ship &ship) = 0;
};

class NullAIFactory : public Oort::CxxAIFactory {
public:
	NullAIFactory() : CxxAIFactory("null") {};
	std::unique_ptr<AI> instantiate(Ship &ship);
};

#if 0
class LuaAIFactory : public AIFactory {
public:
	std::string code;
	virtual std::unique_ptr<AI> instantiate(Ship &ship);
};
#endif

class AI {
public:
	AI(Ship &ship);
	virtual void tick();

	Ship &ship;
};

class CxxAI : public AI {
public:
	CxxAI(Ship &ship);
	virtual void tick();
};

#if 0
class LuaAI : public AI {
public:
	LuaAI(Ship &ship, std::string name, std::string code);
	virtual ~LuaAI();
	virtual void tick();

private:
	lua_State *G, *L;
};
#endif

}

#endif
