class MoveTest : public Test {
public:
	static const int s = 32;
	shared_ptr<Ship> ship;
	Waypoint wpA, wpB, wpC, wpD, wpE;
	float thrust, angular_thrust;

	MoveTest()
		: wpA(this, vec2(0,0), 0.1),
		  wpB(this, vec2(50,0), 0.1),
		  wpC(this, vec2(100,0), 0.1),
		  wpD(this, vec2(100,50), 10),
		  wpE(this, vec2(100,100), 20) {
		AISourceCode ai{"foo.lua", ""};
		auto green = make_shared<Team>("green", ai, vec3(0, 1, 0));
		ship = make_shared<Ship>(this, green);
		ships.push_back(ship);
		thrust = 100 * ship->body->GetMass();
		angular_thrust = M_PI/2 * ship->body->GetInertia();
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
			ship->thrust_angular(angular_thrust);
		} else if (ticks == 3*s) {
			assert_contact(*ship, wpC);
			ship->thrust_angular(-angular_thrust);
		} else if (ticks == 4*s) {
			assert_contact(*ship, wpC);
			ship->thrust_angular(0);
			ship->thrust_main(thrust);
		} else if (ticks == 5*s) {
			assert_contact(*ship, wpD);
			ship->thrust_main(-thrust);
		} else if (ticks == 6*s) {
			assert_contact(*ship, wpE);
			ship->thrust_main(0);
			test_finished = true;
		}
	}
} test;
