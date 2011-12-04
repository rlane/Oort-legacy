// Copyright 2011 Rich Lane
#ifndef OORT_SIM_ENTITY_H_
#define OORT_SIM_ENTITY_H_

#include <memory>

class b2Body;

namespace Oort {

class Game;

class Entity {
	public:
	Game *game;
	b2Body *body;
	bool dead;

	Entity(Game *game);
	~Entity();

	virtual void tick();

	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;
};

}

#endif
