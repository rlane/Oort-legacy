// Copyright 2011 Rich Lane

#ifndef OORT_SIM_GAME_H_
#define OORT_SIM_GAME_H_

#include <list>
#include <memory>
#include <vector>

class b2World;

namespace Oort {

class Ship;
class Bullet;
class Team;
class Scenario;
class AISourceCode;

class Game {
	public:

	Game(Scenario &scn, std::vector<AISourceCode> &ais);
	~Game();

	void tick();
	std::shared_ptr<Team> check_victory();

	std::unique_ptr<b2World> world;
	std::list<std::shared_ptr<Ship>> ships;
	std::list<std::shared_ptr<Bullet>> bullets;
	int ticks;
	float time;
	bool test_finished;

	private:
	void reap();
};

}

#endif
