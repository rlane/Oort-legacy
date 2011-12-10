class MoveTest : public Test {
public:
	static const int s = 32;
	shared_ptr<Ship> ship;
	Waypoint wpA, wpB, wpC;
	float thrust;

	MoveTest()
		: wpA(this, vec2(0,0), 0.1),
		  wpB(this, vec2(50,0), 0.1),
		  wpC(this, vec2(100,0), 0.1) {
		AISourceCode ai{"foo.lua", ""};
		auto green = make_shared<Team>("green", ai, vec3(0, 1, 0));
		ship = make_shared<Ship>(this, green);
		ships.push_back(ship);
		thrust = 100 * ship->body->GetMass();
		ship->thrust_main(thrust);
	}

	void after_tick() {
		if (ticks == 1) {
			assert_contact(*ship, wpA);
		} else if (ticks == 1*s) {
			assert_contact(*ship, wpB);
			ship->thrust_main(-thrust);
		} else if (ticks == 2*s) {
			assert_contact(*ship, wpC);
			ship->thrust_main(0);
			test_finished = true;
		}
	}
} test;
