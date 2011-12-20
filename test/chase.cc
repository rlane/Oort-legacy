#include "test/testcase.h"
#include "glm/gtx/vector_angle.hpp"

class ChaseTest : public Test {
public:
	weak_ptr<Ship> ship, target;
	boost::random::mt19937 prng;
	boost::random::normal_distribution<> v_dist;

	ChaseTest()
	  : prng(42),
	    v_dist(0.0, 100.0) {
		AISourceCode ai{"foo.lua", ""};

		{
			auto team = make_shared<Team>("green", ai, vec3(0, 1, 0));
			auto s = make_shared<Ship>(this, fighter, team);
			ships.push_back(s);
			ship = s;
		}

		{
			auto team = make_shared<Team>("red", ai, vec3(1, 0, 0));
			auto s = make_shared<Ship>(this, fighter, team);
			s->set_position(vec2(100, 0));
			ships.push_back(s);
			target = s;
		}

		ship_ai_tick();
		target_ai_tick();
	}

	float smallest_positive_root_of_quadratic_equation(float a, float b, float c) {
		float z = sqrtf(b*b - 4*a*c);
		float x1 = (b + z)/(2*a);
		float x2 = (b - z)/(2*a);
		if (x1 < 0) {
			return x2;
		} else if (x2 < 0) {
			return x1;
		} else {
			return fminf(x1, x2);
		}
	}

	int accelerated_goto(float p, float v, float a) {
		auto _a = a;
		auto _b = 2*v;
		auto _c = -p + v*v/(2*a);
		auto t = smallest_positive_root_of_quadratic_equation(_a, _b, _c);
		if (t > 0) {
			return 1;
		} else {
			return -1;
		}
	}

	void turn_to(Ship &s, float angle) {
		auto h = s.get_heading();
		auto w = s.get_angular_velocity();
		auto wa = 2.0f;
		auto diff = angle_diff(h, angle);
		auto f = accelerated_goto(diff, -w, wa);
		s.acc_angular(f * wa);
	}

	void drive_towards(Ship &s, vec2 tp, float speed) {
		auto p = s.get_position();
		auto v = s.get_velocity();
		auto h = s.get_heading();
		auto a = radians(orientedAngle(vec2(1,0), normalize(tp-p)));
		auto rv = rotate(v, degrees(-h));
		s.acc_lateral(-1*rv.y);
		turn_to(s, a);

		float max_main_acc = 10;
		auto diff = angle_diff(a,h);
		if (rv.x > speed) {
			s.acc_main(speed-rv.x);
		} else if (fabsf(diff) < M_PI/4) {
			s.acc_main(max_main_acc);
		} else if (fabsf(diff) > 3*M_PI/4) {
			s.acc_main(-max_main_acc);
		} else {
			s.acc_main(0);
		}
	}

	void ship_ai_tick() {
		auto ship_ref = ship.lock(); 
		auto target_ref = target.lock(); 

		if (!target_ref) {
			drive_towards(*ship_ref, vec2(0,0), 10);
			return;
		}

		auto dp = target_ref->get_position() - ship_ref->get_position();
		auto th = radians(orientedAngle(vec2(1,0), normalize(dp)));

		if (ticks % 4 == 0) {
			ship_ref->fire(th);
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
