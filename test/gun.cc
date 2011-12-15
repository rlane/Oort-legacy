class GunTest : public Test {
public:
	shared_ptr<Ship> shipA, shipB;
	unique_ptr<ShipClass> target;

	GunTest() {
		AISourceCode ai{"foo.lua", ""};
		auto blue = make_shared<Team>("blue", ai, vec3(0, 0, 1));
		auto red = make_shared<Team>("red", ai, vec3(1, 0, 0));

		target = unique_ptr<ShipClass>(new ShipClass("target", fighter->vertices, 10e3, 10e3));

		{
			shipA = make_shared<Ship>(this, fighter, blue);
			shipA->body->SetTransform(b2Vec2(0, 0), 0);
			shipA->body->SetLinearVelocity(b2Vec2(0, 0));
			ships.push_back(shipA);
		}

		{
			shipB = make_shared<Ship>(this, target.get(), red);
			shipB->body->SetTransform(b2Vec2(10, 0), M_PI/2);
			shipB->body->SetLinearVelocity(b2Vec2(0, 0));
			ships.push_back(shipB);
		}
	}

	void after_tick() {
		if (shipB->dead) {
			test_finished = true;
		} else {
			shipA->fire(0);
		}
	}
} test;
