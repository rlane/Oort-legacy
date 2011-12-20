#include "test/testcase.h"

class BallTest : public Test {
public:
	BallTest() {
		boost::random::mt19937 prng(42);
		boost::random::normal_distribution<> p_dist(0.0, 10000.0);
		boost::random::normal_distribution<> v_dist(0.0, 20.0);

		AISourceCode ai{"foo.lua", ""};
		auto red = make_shared<Team>("red", ai, vec3(1, 0, 0));
		auto green = make_shared<Team>("green", ai, vec3(0, 1, 0));
		auto blue = make_shared<Team>("blue", ai, vec3(0, 0, 1));
		vector<shared_ptr<Team>> teams = { red, green, blue };

		for (auto i = 0; i < 100; i++) {
			auto ship = make_shared<Ship>(this, fighter, teams[i % teams.size()]);
			ship->set_position(vec2(p_dist(prng), p_dist(prng)));
			ship->set_velocity(vec2(v_dist(prng), v_dist(prng)));
			ships.push_back(ship);
		}
	}

	shared_ptr<Ship> find_target(Ship &s) {
		BOOST_FOREACH(auto t, ships) {
			if (t->team != s.team) {
				return t;
			}
		}
		return nullptr;
	}

	void after_tick() {
		if (check_victory()) {
			test_finished = true;
		}

		BOOST_FOREACH(auto ship, ships) {
			auto t = find_target(*ship);

			if (t) {
				auto p = ship->get_position();
				auto th = angle_between(p, t->get_position());
				drive_towards(*ship, t->get_position(), 1000);

				if (ticks % 4 == 0) {
					ship->fire(th);
				}
			} else {
				drive_towards(*ship, vec2(0,0), 100);
			}
		}
	}
} test;
