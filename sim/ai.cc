// Copyright 2011 Rich Lane
#include "sim/ai.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>

#if 0
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#endif

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

class NullAI : public CxxAI {
public:
	NullAI(Ship &ship) : CxxAI(ship) {};
	void tick() {};
};

std::unique_ptr<AI> NullAIFactory::instantiate(Ship &ship) {
	return std::unique_ptr<AI>(new NullAI(ship));
}

}
