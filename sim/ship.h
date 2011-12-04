// Copyright 2011 Rich Lane
#ifndef OORT_SIM_SHIP_H_
#define OORT_SIM_SHIP_H_

#include <memory>

class b2Body;

namespace Oort {

class Game;
class Team;
class AI;

class Ship {
	public:
	Game *game;
	std::shared_ptr<Team> team;
	std::unique_ptr<AI> ai;
	b2Body *body;

	Ship(Game *game, std::shared_ptr<Team> team);
	~Ship();

	void tick();

	Ship(const Ship&) = delete;
	Ship& operator=(const Ship&) = delete;
};

}

#endif
