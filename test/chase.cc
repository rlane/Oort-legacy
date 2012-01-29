#include "test/testcase.h"
#include "glm/gtx/vector_angle.hpp"
#include <math.h>

class ChaseAI : public CxxAI {
public:
	ProportionalNavigator nav;

	ChaseAI(Ship &ship)
		: CxxAI(ship),
		  nav(ship, 5, ship.klass.max_main_acc) {}

	void tick() {
		auto target = find_target(ship);

		if (!target) {
			drive_towards(ship, vec2(0,0), 10);
			return;
		}

		if (&ship.klass == fighter.get()) {
			const GunDef &gun = ship.klass.guns[0];
			auto a = lead(ship.get_position(), target->get_position(),
			              ship.get_velocity(), target->get_velocity(),
			              gun.velocity, gun.ttl);
			if (!isnan(a)) {
				ship.fire_gun(0, a);
			}
			if (ship.game->ticks % 32 == 0) {
				ship.fire_missile(target);
			}

			drive_towards(ship, target->get_position(), ship.klass.max_main_acc*2);
		} else {
			nav.seek(target->get_position(), target->get_velocity());
		}
	}
};

class TargetAI : public CxxAI {
public:
	int countdown;
	boost::random::mt19937 prng;

	TargetAI(Ship &ship)
	  : CxxAI(ship),
	    countdown(0),
	    prng(42) {}

	void tick() {
		if (length(ship.get_position()) > 10000) {
			drive_towards(ship, vec2(0, 0), ship.klass.max_main_acc*2);
			return;
		}

		if (countdown == 0) {
			boost::random::normal_distribution<> main_dist(0, ship.klass.max_main_acc/3);
			boost::random::normal_distribution<> lateral_dist(0, ship.klass.max_lateral_acc/3);
			boost::uniform_real<> heading_dist(0, 2*M_PI);
			countdown = rand() % 100;
			ship.acc_main(main_dist(prng));
			ship.acc_lateral(lateral_dist(prng));
			ship.set_heading(heading_dist(prng));
		} else {
			countdown--;
		}
	}
};

class ChaseTest : public SimpleTest {
public:
	ChaseTest()
		: SimpleTest(Scenario::load("test/chase.json"),
				         { CxxAI::factory<ChaseAI>(), CxxAI::factory<TargetAI>() })
	{
		game->radius = 100000;
	}

	void after_tick() {
		Team *winner;
		if (game->ships.empty() || game->check_victory(winner)) {
			finished = true;
		}
	}
} test;
