#include "test/testcase.h"
#include "sim/model.h"
#include <boost/bind.hpp>

class GunAI : public CxxAI {
public:
	GunAI(Ship &ship) : CxxAI(ship) {}

	void tick() {
		ship.fire_gun(0, 0);
	}
};

class GunTest : public SimpleTest {
public:
	GunTest()
		: SimpleTest(Scenario::load("test/gun.json"),
		             { CxxAI::factory<GunAI>(), CxxAI::factory<CxxAI>() })
	{
	}

	void after_tick() {
		Team *winner;
		if (game->ships.empty() || game->check_victory(winner)) {
			finished = true;
		}
	}
} test;
