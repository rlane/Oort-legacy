#include "test/testcase.h"
#include "glm/gtx/vector_angle.hpp"

class ChaseAI : public CxxAI {
public:
	weak_ptr<Ship> target_weak;

	ChaseAI(Ship &ship, weak_ptr<Ship> target_weak)
	  : CxxAI(ship),
	    target_weak(target_weak) {}

	void tick() {
		auto target = target_weak.lock(); 

		if (!target) {
			drive_towards(ship, vec2(0,0), 10);
			return;
		}

		const GunDef &gun = ship.klass.guns[0];
		auto a = lead(ship.get_position(), target->get_position(),
									ship.get_velocity(), target->get_velocity(),
									gun.velocity, gun.ttl);
		if (!isnan(a)) {
			ship.fire_gun(0, a);
		}

		drive_towards(ship, target->get_position(), 100);
	}
};

class TargetAI : public CxxAI {
public:
	int countdown;
	boost::random::mt19937 prng;
	boost::random::normal_distribution<> v_dist;

	TargetAI(Ship &ship)
	  : CxxAI(ship),
	    countdown(0),
	    prng(42),
	    v_dist(0.0, 300.0) {}

	void tick() {
		if (countdown == 0) {
			countdown = rand() % 100;
			auto v = vec2(v_dist(prng), v_dist(prng));
			ship.set_velocity(v);
			ship.set_heading(radians(orientedAngle(vec2(1,0), normalize(v))));
		} else {
			countdown--;
		}
	}
};

class ChaseAIFactory : public CxxAIFactory {
public:
	weak_ptr<Ship> target_weak;

	ChaseAIFactory(weak_ptr<Ship> target_weak)
	  : CxxAIFactory("chase"),
	    target_weak(target_weak) {};

	unique_ptr<AI> instantiate(Ship &ship) {
		return unique_ptr<AI>(new ChaseAI(ship, target_weak));
	}
};

class TargetAIFactory : public CxxAIFactory {
public:
	TargetAIFactory() : CxxAIFactory("target") {};

	unique_ptr<AI> instantiate(Ship &ship) {
		return unique_ptr<AI>(new TargetAI(ship));
	}
};

class ChaseTest : public Test {
public:
	weak_ptr<Ship> target_weak;

	ChaseTest() {
		{
			auto team = make_shared<Team>("red", make_shared<TargetAIFactory>(), vec3(1, 0, 0));
			auto s = make_shared<Ship>(this, *fighter, team);
			s->set_position(vec2(1500, 0));
			ships.push_back(s);
			target_weak = s;
		}

		{
			auto team = make_shared<Team>("green", make_shared<ChaseAIFactory>(target_weak), vec3(0, 1, 0));
			auto s = make_shared<Ship>(this, *fighter, team);
			ships.push_back(s);
		}
	}

	void after_tick() {
		if (target_weak.expired()) {
			test_finished = true;
		}
	}
} test;
