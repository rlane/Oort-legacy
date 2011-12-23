#include "test/testcase.h"
#include <math.h>

class BeamAI : public CxxAI {
public:
	BeamAI(Ship &ship) : CxxAI(ship) {}

	void tick() {
		auto t = find_target(ship);
		if (t) {
			drive_towards(ship, t->get_position(), 100);
			ship.fire_beam(0, ship.get_heading());
		} else {
			drive_towards(ship, vec2(0,0), 100);
		}
	}
};

class BeamTest : public Test {
	static constexpr float dist = 600;

public:
	BeamTest() {
		auto green = make_shared<Team>("green", CxxAI::factory<BeamAI>(), vec3(0, 1, 0));
		auto red = make_shared<Team>("red", CxxAI::factory<CxxAI>(), vec3(1, 0, 0));

		{
			auto ship = make_shared<Ship>(this, *ion_cannon_frigate, green);
			ship->set_position(vec2(-dist, 0));
			ship->set_velocity(vec2(0, 0));
			ship->set_heading(0);
			ships.push_back(ship);
		}

		{
			auto ship = make_shared<Ship>(this, *fighter, red);
			ship->set_position(vec2(dist, 0));
			ship->set_velocity(vec2(0, 0));
			ship->set_heading(M_PI);
			ships.push_back(ship);
		}
	}

	void after_tick() {
		if (ships.empty() || check_victory()) {
			test_finished = true;
		}
	}
} test;
