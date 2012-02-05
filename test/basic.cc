#include "test/testcase.h"

class BasicTest : public SimpleTest {
public:
	BasicTest()
		: SimpleTest(Scenario::load("scenarios/basic.json"),
				         { builtin_ai_factory, builtin_ai_factory })
	{
	}

	void after_tick() {
		Team *winner;
		if (game->ships.empty() || game->check_victory(winner)) {
			finished = true;
		}
	}
} test;
