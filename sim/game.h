// Copyright 2011 Rich Lane

#ifndef OORT_SIM_GAME_H_
#define OORT_SIM_GAME_H_

#include <stdint.h>
#include <list>
#include <memory>
#include <vector>
#include "glm/glm.hpp"
#include "sim/explosion.h"

class b2World;

namespace Oort {

class Ship;
class Bullet;
class Beam;
class Team;
class Scenario;
class AIFactory;
class Weapon;

struct Hit {
	Ship *ship;
	Weapon *weapon;
	glm::vec2 cp;
	float e;
};

class Game {
	public:

	Game(const Scenario &scn, const std::vector<std::shared_ptr<AIFactory>> &ai_factories);
	~Game();

	void tick();
	virtual void after_tick();
	std::shared_ptr<Team> check_victory();
	std::shared_ptr<Ship> lookup_ship(uint32_t id);

	std::unique_ptr<b2World> world;
	std::list<std::shared_ptr<Ship>> ships;
	std::list<std::shared_ptr<Bullet>> bullets;
	std::list<std::shared_ptr<Beam>> beams;
	std::vector<Hit> hits;
	std::vector<Explosion> explosions;
	int ticks;
	float time;
	bool test_finished;
	float radius;

	private:
	void reap();
};

}

#endif
