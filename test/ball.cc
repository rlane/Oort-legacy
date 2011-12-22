#include "test/testcase.h"
#include <math.h>

class BallTest : public Test {
public:
	BallTest() {
		boost::random::mt19937 prng(42);
		boost::random::normal_distribution<> p_dist(0.0, 1000.0);
		boost::random::normal_distribution<> v_dist(0.0, 20.0);

		AISourceCode ai{"foo.lua", ""};
		auto red = make_shared<Team>("red", ai, vec3(1, 0, 0));
		auto green = make_shared<Team>("green", ai, vec3(0, 1, 0));
		auto blue = make_shared<Team>("blue", ai, vec3(0, 0, 1));
		vector<shared_ptr<Team>> teams = { red, green, blue };
		vector<ShipClass*> klasses = { 
			fighter.get(),
			fighter.get(),
			fighter.get(),
			ion_cannon_frigate.get(),
			assault_frigate.get(),
		};

		for (auto i = 0; i < 100; i++) {
			auto ship = make_shared<Ship>(this, *klasses[(i/teams.size()) % klasses.size()], teams[i % teams.size()]);
			ship->set_position(vec2(p_dist(prng), p_dist(prng)));
			ship->set_velocity(vec2(v_dist(prng), v_dist(prng)));
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
				drive_towards(*ship, t->get_position(), ship->klass.max_main_acc*5);

				const GunDef &gun = ship->klass.guns[0];
				auto a = lead(ship->get_position(), t->get_position(),
											ship->get_velocity(), t->get_velocity(),
											gun.velocity, gun.ttl);
				if (!isnan(a)) {
					if (gun.coverage == 0 && fabsf(angle_diff(ship->get_heading(), a) < 0.1)) {
						ship->fire_gun(0, ship->get_heading());
					} else {
						ship->fire_gun(0, a);
					}
				}
			} else {
				drive_towards(*ship, vec2(0,0), ship->klass.max_main_acc*2);
			}
		}
	}
} test;
