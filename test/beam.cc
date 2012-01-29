#include "test/testcase.h"

class BeamAI : public CxxAI {
public:
	BeamAI(Ship &ship) : CxxAI(ship) {}

	void tick() {
		auto t = find_target(ship);
		if (t) {
			drive_towards(ship, t->get_position(), 100);
			ship.fire_beam(0, ship.get_heading());
		} else {
			drive_towards(ship, vec2(0,0), 100);
		}
	}
};

class BeamTest : public SimpleTest {
	static constexpr float dist = 600;

public:
	BeamTest()
		: SimpleTest(Scenario::load("test/beam.json"),
		             { CxxAI::factory<BeamAI>(), CxxAI::factory<CxxAI>() })
	{
	}

	void after_tick() {
		Team *winner;
		if (game->ships.empty() || game->check_victory(winner)) {
			finished = true;
		}
	}
} test;
