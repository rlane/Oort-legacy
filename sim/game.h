// Copyright 2011 Rich Lane

#ifndef OORT_SIM_GAME_H_
#define OORT_SIM_GAME_H_

#include <stdint.h>
#include <list>
#include <memory>
#include <vector>
#include "glm/glm.hpp"
#include "sim/explosion.h"
#include "common/constexpr.h"

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
	static constexpr float tick_length = 1.0f/32;

	Game(const Scenario &scn, const std::vector<std::shared_ptr<AIFactory>> &ai_factories);
	~Game();

	void tick();
	bool check_victory(Team *&team);
	std::shared_ptr<Ship> lookup_ship(uint32_t id);

	std::unique_ptr<b2World> world;
	std::list<std::shared_ptr<Ship>> ships;
	std::list<std::shared_ptr<Bullet>> bullets;
	std::list<std::shared_ptr<Beam>> beams;
	std::vector<Hit> hits;
	std::vector<Explosion> explosions;
	int ticks;
	float time;
	float radius;

	private:
	void reap();
};

}

#endif
