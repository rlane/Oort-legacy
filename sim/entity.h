// Copyright 2011 Rich Lane
#ifndef OORT_SIM_ENTITY_H_
#define OORT_SIM_ENTITY_H_

#include <memory>

class b2Body;

namespace Oort {

class Game;
class Team;

class Entity {
	public:
	Game *game;
	std::shared_ptr<Team> team;
	b2Body *body;
	bool dead;

	Entity(Game *game, std::shared_ptr<Team> team);
	~Entity();

	virtual void tick();

	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;
};

}

#endif
