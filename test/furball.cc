#include "test/testcase.h"

class FurballTest : public SimpleTest {
public:
	FurballTest()
		: SimpleTest(Scenario::load("test/furball.json"),
				         { builtin_ai_factory, builtin_ai_factory, builtin_ai_factory })
	{
	}

	void after_tick() {
		Team *winner;
		if (game->ships.empty() || game->check_victory(winner)) {
			finished = true;
		}
	}
} test;
