#include "test/testcase.h"

class MoveTest : public Test {
public:
	static const int s = 32;
	static constexpr float d = 100;
	static constexpr float main_acc = d;
	static constexpr float angular_acc = M_PI/2;
	shared_ptr<Ship> ship;
	Waypoint wpA, wpB, wpC, wpD, wpE, wpF, wpG, wpH;
	float thrust, angular_thrust;

	MoveTest()
		: wpA(this, vec2(0,0), 0.1),
		  wpB(this, vec2(d/2,0), 0.1),
		  wpC(this, vec2(d,0), 0.1),
		  wpD(this, vec2(d,d/2), 0.1),
		  wpE(this, vec2(d,d), 0.1),
		  wpF(this, vec2(d/2,d), 0.1),
		  wpG(this, vec2(0,d), 0.1),
		  wpH(this, vec2(0,d/2), 0.1) {
		AISourceCode ai{"foo.lua", ""};
		auto green = make_shared<Team>("green", ai, vec3(0, 1, 0));
		ship = make_shared<Ship>(this, fighter, green);
		ships.push_back(ship);
		ship->acc_main(main_acc);
	}

	void after_tick() {
		if (ticks == 1) {
			assert_contact(*ship, wpA);
		} else if (ticks == 1*s) {
			assert_contact(*ship, wpB);
			ship->acc_main(-main_acc);
		} else if (ticks == 2*s) {
			assert_contact(*ship, wpC);
			ship->acc_main(0);
			ship->acc_angular(angular_acc);
		} else if (ticks == 3*s) {
			assert_contact(*ship, wpC);
			ship->acc_angular(-angular_acc);
		} else if (ticks == 4*s) {
			assert_contact(*ship, wpC);
			ship->acc_angular(0);
			ship->acc_main(main_acc);
		} else if (ticks == 5*s) {
			assert_contact(*ship, wpD);
			ship->acc_main(-main_acc);
		} else if (ticks == 6*s) {
			assert_contact(*ship, wpE);
			ship->acc_main(0);
			ship->acc_lateral(main_acc); // XXX
		} else if (ticks == 7*s) {
			assert_contact(*ship, wpF);
			ship->acc_lateral(-main_acc); // XXX
		} else if (ticks == 8*s) {
			assert_contact(*ship, wpG);
			ship->acc_lateral(0);
			ship->acc_angular(angular_acc*2);
		} else if (ticks == 9*s) {
			assert_contact(*ship, wpG);
			ship->acc_angular(-angular_acc*2);
		} else if (ticks == 10*s) {
			assert_contact(*ship, wpG);
			ship->acc_angular(0);
			ship->acc_main(main_acc);
		} else if (ticks == 11*s) {
			assert_contact(*ship, wpH);
			ship->acc_main(-main_acc);
		} else if (ticks == 12*s) {
			assert_contact(*ship, wpA);
			ship->acc_main(0);
			test_finished = true;
		}
	}
} test;
