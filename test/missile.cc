#include "test/testcase.h"
#include "sim/model.h"
#include <boost/bind.hpp>

class MissileAI : public CxxAI {
public:
	MissileAI(Ship &ship) : CxxAI(ship) {}

	void tick() {
		auto t = find_target(ship);
		if (&ship.klass == fighter.get()) {
			if (t && ship.game->ticks % 8 == 0) {
				ship.fire_missile(t);
			}
		} else if (&ship.klass == missile.get()) {
			if (t) {
				drive_towards(ship, t->get_position(), ship.klass.max_main_acc*5);
			} else {
				ship.explode();
			}
		}
	}
};

class MissileTest : public SimpleTest {
public:
	MissileTest()
		: SimpleTest(Scenario::load("test/missile.json"),
				         { CxxAI::factory<MissileAI>(), CxxAI::factory<CxxAI>() })
	{
	}

	void after_tick() {
		Team *winner;
		if (game->ships.empty() || game->check_victory(winner)) {
			finished = true;
		}
	}
} test;
