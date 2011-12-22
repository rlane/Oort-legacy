#include "test/testcase.h"
#include "sim/model.h"

class GunAI : public CxxAI {
public:
	GunAI(Ship &ship) : CxxAI(ship) {}

	void tick() {
		ship.fire_gun(0, 0);
	}
};

class GunAIFactory : public CxxAIFactory {
public:
	GunAIFactory() : CxxAIFactory("gun") {};

	unique_ptr<AI> instantiate(Ship &ship) {
		return unique_ptr<AI>(new GunAI(ship));
	}
};

class GunTest : public Test {
public:
	weak_ptr<Ship> shipB;

	GunTest() {
		auto blue = make_shared<Team>("blue", make_shared<GunAIFactory>(), vec3(0, 0, 1));
		auto red = make_shared<Team>("red", make_shared<NullAIFactory>(), vec3(1, 0, 0));

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
