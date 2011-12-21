#include "test/testcase.h"
#include "glm/gtx/vector_angle.hpp"

class ChaseTest : public Test {
public:
	weak_ptr<Ship> ship, target;
	boost::random::mt19937 prng;
	boost::random::normal_distribution<> v_dist;

	ChaseTest()
	  : prng(42),
	    v_dist(0.0, 300.0) {
		AISourceCode ai{"foo.lua", ""};

		{
			auto team = make_shared<Team>("green", ai, vec3(0, 1, 0));
			auto s = make_shared<Ship>(this, *fighter, team);
			ships.push_back(s);
			ship = s;
		}

		{
			auto team = make_shared<Team>("red", ai, vec3(1, 0, 0));
			auto s = make_shared<Ship>(this, *fighter, team);
			s->set_position(vec2(1500, 0));
			ships.push_back(s);
			target = s;
		}

		ship_ai_tick();
		target_ai_tick();
	}

	void ship_ai_tick() {
		auto ship_ref = ship.lock(); 
		auto target_ref = target.lock(); 

		if (!target_ref) {
			drive_towards(*ship_ref, vec2(0,0), 10);
			return;
		}

		if (ticks % 4 == 0) {
			const GunDef &gun = ship_ref->klass.guns[0];
			auto a = lead(ship_ref->get_position(), target_ref->get_position(),
			              ship_ref->get_velocity(), target_ref->get_velocity(),
			              gun.velocity, gun.ttl);
			if (!isnan(a)) {
				ship_ref->fire(a);
			}
		}

		drive_towards(*ship_ref, target_ref->get_position(), 100);
	}

	void target_ai_tick() {
		auto target_ref = target.lock(); 
		if (!target_ref) {
			return;
		}

		static int countdown;
		if (countdown == 0) {
			countdown = rand() % 100;
			auto v = vec2(v_dist(prng), v_dist(prng));
			target_ref->set_velocity(v);
			target_ref->set_heading(radians(orientedAngle(vec2(1,0), normalize(v))));
		} else {
			countdown--;
		}
	}

	void after_tick() {
		ship_ai_tick();
		target_ai_tick();

		if (target.expired()) {
			test_finished = true;
		}
	}
} test;
