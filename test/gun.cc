#include "test/testcase.h"

class GunTest : public Test {
public:
	weak_ptr<Ship> shipA, shipB;
	unique_ptr<ShipClass> target;

	GunTest() {
		AISourceCode ai{"foo.lua", ""};
		auto blue = make_shared<Team>("blue", ai, vec3(0, 0, 1));
		auto red = make_shared<Team>("red", ai, vec3(1, 0, 0));

		target = unique_ptr<ShipClass>(new ShipClass("target", fighter->vertices, 10e3, 10e3));

		{
			auto tmpA = make_shared<Ship>(this, fighter, blue);
			tmpA->body->SetTransform(b2Vec2(0, 0), 0);
			tmpA->body->SetLinearVelocity(b2Vec2(0, 0));
			ships.push_back(tmpA);
			shipA = tmpA;
		}

		{
			auto tmpB = make_shared<Ship>(this, target.get(), red);
			tmpB->body->SetTransform(b2Vec2(10, 0), M_PI/2);
			tmpB->body->SetLinearVelocity(b2Vec2(0, 0));
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
