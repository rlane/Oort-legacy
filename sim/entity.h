// Copyright 2011 Rich Lane
#ifndef OORT_SIM_ENTITY_H_
#define OORT_SIM_ENTITY_H_

#include <memory>
#include "glm/glm.hpp"

class b2Body;

namespace Oort {

class Game;
class Team;
class ContactListener;

class Entity {
public:
	Game *game;
	std::shared_ptr<Team> team;
	bool dead;
	float mass;

	Entity(Game *game, std::shared_ptr<Team> team);
	~Entity();

	virtual void tick();
	virtual bool is_weapon();

	void set_position(glm::vec2 p);
	glm::vec2 get_position() const;

	void set_velocity(glm::vec2 v);
	glm::vec2 get_velocity() const;

	void set_heading(float angle);
	float get_heading() const;

	void set_angular_velocity(float w);
	float get_angular_velocity() const;

	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;

protected:
	b2Body *body;
	friend void Oort::assert_contact(const Entity &a, const Entity &b);
};

}

#endif
