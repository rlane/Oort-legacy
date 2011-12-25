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
				if (length(ship.get_position() - t->get_position()) < 40) {
					ship.explode();
				}
			} else {
				ship.explode();
			}
		}
	}
};

class MissileTest : public Test {
public:
	weak_ptr<Ship> shipB;

	MissileTest() {
		auto blue = make_shared<Team>("blue", CxxAI::factory<MissileAI>(), vec3(0, 0, 1));
		auto red = make_shared<Team>("red", CxxAI::factory<CxxAI>(), vec3(1, 0, 0));

		{
			auto tmpA = make_shared<Ship>(this, *fighter, blue);
			tmpA->set_position(vec2(0, 0));
			tmpA->set_heading(0);
			tmpA->set_velocity(vec2(0,0));
			ships.push_back(tmpA);
		}

		{
			auto tmpB = make_shared<Ship>(this, *ion_cannon_frigate, red);
			tmpB->set_position(vec2(500, 0));
			tmpB->set_heading(M_PI/2);
			tmpB->set_velocity(vec2(0,0));
			ships.push_back(tmpB);
			shipB = tmpB;
		}
	}

	void after_tick() {
		if (shipB.expired()) {
			test_finished = true;
		}
	}
} test;
