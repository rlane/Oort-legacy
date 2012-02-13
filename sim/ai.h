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
	virtual std::unique_ptr<AI> instantiate(Ship &ship) = 0;
};

template <class T>
class CxxAIFactory : public AIFactory {
public:
	CxxAIFactory() : AIFactory() {}

	std::unique_ptr<AI> instantiate(Ship &ship) {
		return std::unique_ptr<AI>(new T(ship));
	}
};

class LuaAIFactory : public AIFactory {
public:
	std::string filename;
	std::string code;
	LuaAIFactory(std::string filename, std::string code);
	virtual std::unique_ptr<AI> instantiate(Ship &ship);
};

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

	template <class T>
	static std::shared_ptr<AIFactory> factory() {
		return std::make_shared<CxxAIFactory<T>>();
	}
};

class LuaAI : public AI {
public:
	LuaAI(Ship &ship, std::string filename, std::string code);
	virtual ~LuaAI();
	virtual void tick();
	void register_api();

private:
	lua_State *G, *L;
};

}

#endif
