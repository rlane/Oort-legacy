#include "test/testcase.h"

static const int s = 32;
static const float d = 100;
static const float main_acc = d;
static const float lateral_acc = d/4;
static const float angular_acc = M_PI/2;

enum {
	t1 = 0,        // at wpA, main acc start
	t2 = t1+s,     // at wpB, main acc reverse
	t3 = t2+s,     // at wpC, main acc end, angular acc start
	t4 = t3+s,     // at wpC, angular acc reverse
	t5 = t4+s,     // at wpC, angular acc end, main acc start
	t6 = t5+s,     // at wpD, main acc reverse
	t7 = t6+s,     // at wpE, main acc end, lateral acc start
	t8 = t7+2*s,   // at wpF, lateral acc reverse
  t9 = t8+2*s,   // at wpG, lateral acc end, angular acc start (2x)
	t10 = t9+s,    // at wpG, angular acc reverse
	t11 = t10+s,   // at wpG, angular acc end, main acc start (64x)
	t12 = t11+s/8, // at wpH, main acc reverse
	t13 = t12+s/8, // at wpA, main acc end
};

class MoveAI : public CxxAI {
public:
	MoveAI(Ship &ship) : CxxAI(ship) {}

	void tick() {
		auto ticks = ship.game->ticks;
		if (ticks == t1) {
			ship.acc_main(main_acc);
		} else if (ticks == t2) {
			ship.acc_main(-main_acc);
		} else if (ticks == t3) {
			ship.acc_main(0);
			ship.acc_angular(angular_acc);
		} else if (ticks == t4) {
			ship.acc_angular(-angular_acc);
		} else if (ticks == t5) {
			ship.acc_angular(0);
			ship.acc_main(main_acc);
		} else if (ticks == t6) {
			ship.acc_main(-main_acc);
		} else if (ticks == t7) {
			ship.acc_main(0);
			ship.acc_lateral(lateral_acc);
		} else if (ticks == t8) {
			ship.acc_lateral(-lateral_acc);
		} else if (ticks == t9) {
			ship.acc_lateral(0);
			ship.acc_angular(angular_acc*2);
		} else if (ticks == t10) {
			ship.acc_angular(-angular_acc*2);
		} else if (ticks == t11) {
			ship.acc_angular(0);
			ship.acc_main(main_acc*64);
		} else if (ticks == t12) {
			ship.acc_main(-main_acc*64);
		} else if (ticks == t13) {
			ship.acc_main(0);
		}
	}
};

class MoveTest : public Test {
public:
	shared_ptr<Ship> ship;
	Waypoint wpA, wpB, wpC, wpD, wpE, wpF, wpG, wpH;
	unique_ptr<ShipClass> speedy;

	MoveTest()
		: wpA(this, vec2(0,0), 0.1),
		  wpB(this, vec2(d/2,0), 0.1),
		  wpC(this, vec2(d,0), 0.1),
		  wpD(this, vec2(d,d/2), 0.1),
		  wpE(this, vec2(d,d), 0.1),
		  wpF(this, vec2(d/2,d), 0.1),
		  wpG(this, vec2(0,d), 0.1),
		  wpH(this, vec2(0,d/2), 0.1)
	{
		{
			ShipClassDef def = *fighter;
			def.name = "speedy";
			def.max_main_acc = main_acc*64;
			def.max_lateral_acc = lateral_acc;
			def.max_angular_acc = angular_acc*2;
			speedy = unique_ptr<ShipClass>(new ShipClass(def));
		}

		auto green = make_shared<Team>("green", CxxAI::factory<MoveAI>(), vec3(0, 1, 0));
		ship = make_shared<Ship>(this, *speedy, green);
		ships.push_back(ship);
	}

	void after_tick() {
		if (ticks == 1) {
			assert_contact(*ship, wpA);
		} else if (ticks == t2) {
			assert_contact(*ship, wpB);
		} else if (ticks == t3) {
			assert_contact(*ship, wpC);
		} else if (ticks == t4) {
			assert_contact(*ship, wpC);
		} else if (ticks == t5) {
			assert_contact(*ship, wpC);
		} else if (ticks == t6) {
			assert_contact(*ship, wpD);
		} else if (ticks == t7) {
			assert_contact(*ship, wpE);
		} else if (ticks == t8) {
			assert_contact(*ship, wpF);
		} else if (ticks == t9) {
			assert_contact(*ship, wpG);
		} else if (ticks == t10) {
			assert_contact(*ship, wpG);
		} else if (ticks == t11) {
			assert_contact(*ship, wpG);
		} else if (ticks == t12) {
			assert_contact(*ship, wpH);
		} else if (ticks == t13) {
			assert_contact(*ship, wpA);
			test_finished = true;
		}
	}
} test;
