#include "test/testcase.h"
#include <math.h>

class JoustTest : public Test {
	static constexpr float dist = 1000;
	static constexpr float speed = 500;

public:
	JoustTest() {
		AISourceCode ai{"foo.lua", ""};
		auto green = make_shared<Team>("green", ai, vec3(0, 1, 0));
		auto red = make_shared<Team>("red", ai, vec3(1, 0, 0));

		{
			auto ship = make_shared<Ship>(this, *fighter, green);
			ship->set_position(vec2(-dist, 0));
			ship->set_velocity(vec2(speed, 0));
			ship->set_heading(0);
			ships.push_back(ship);
		}

		{
			auto ship = make_shared<Ship>(this, *fighter, red);
			ship->set_position(vec2(dist, 0));
			ship->set_velocity(vec2(-speed, 0));
			ship->set_heading(M_PI);
			ships.push_back(ship);
		}
	}

	shared_ptr<Ship> find_target(Ship &s) {
		shared_ptr<Ship> target;
		float dist = 1e9f;
		BOOST_FOREACH(auto t, ships) {
			if (t->team != s.team) {
				float d = length(t->get_position() - s.get_position());
				if (d < dist) {
					target = t;
					dist = d;
				}
			}
		}
		return target;
	}

	void after_tick() {
		if (ships.empty() || check_victory()) {
			test_finished = true;
		}

		BOOST_FOREACH(auto ship, ships) {
			auto t = find_target(*ship);

			if (t) {
				drive_towards(*ship, t->get_position(), speed);
				const GunDef &gun = ship->klass.guns[0];
				auto a = lead(ship->get_position(), t->get_position(),
				              ship->get_velocity(), t->get_velocity(),
				              gun.velocity, gun.ttl);
				if (!isnan(a)) {
					ship->fire(a);
				}
			} else {
				drive_towards(*ship, vec2(0,0), 100);
			}
		}
	}
} test;
