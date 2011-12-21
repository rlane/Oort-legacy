#include "test/testcase.h"

class GunTest : public Test {
public:
	weak_ptr<Ship> shipA, shipB;
	unique_ptr<ShipClass> target;

	GunTest() {
		AISourceCode ai{"foo.lua", ""};
		auto blue = make_shared<Team>("blue", ai, vec3(0, 0, 1));
		auto red = make_shared<Team>("red", ai, vec3(1, 0, 0));

		{
			ShipClassDef def;
			def.name = "target";
			def.mass = 10e3;
			def.hull = 100e3;
			def.max_main_acc = 0;
			def.max_lateral_acc = 0;
			def.max_angular_acc = 0;
			def.model = fighter->model;
			target = unique_ptr<ShipClass>(new ShipClass(def));
		}

		{
			auto tmpA = make_shared<Ship>(this, *fighter, blue);
			tmpA->set_position(vec2(00, 0));
			tmpA->set_heading(0);
			tmpA->set_velocity(vec2(0,0));
			ships.push_back(tmpA);
			shipA = tmpA;
		}

		{
			auto tmpB = make_shared<Ship>(this, *target, red);
			tmpB->set_position(vec2(10, 0));
			tmpB->set_heading(M_PI/2);
			tmpB->set_velocity(vec2(0,0));
			ships.push_back(tmpB);
			shipB = tmpB;
		}
	}

	void after_tick() {
		cout << "use count: " << shipB.use_count() << endl;
		auto tmpB = shipB.lock();
		if (!tmpB) {
			test_finished = true;
		} else {
			cout << "dead: " << tmpB->dead << endl;
			shipA.lock()->fire(0);
		}
	}
} test;
