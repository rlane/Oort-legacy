// Copyright 2011 Rich Lane
#include "sim/game.h"

#include <memory>
#include <vector>
#include <unordered_set>
#include <typeinfo>
#include <boost/foreach.hpp>

#include "glm/glm.hpp"
#include <Box2D/Box2D.h>

#include "sim/ship.h"
#include "sim/bullet.h"
#include "sim/scenario.h"
#include "sim/ai.h"
#include "sim/team.h"

using std::shared_ptr;
using std::vector;
using std::make_shared;

namespace Oort {

class ContactFilter : public b2ContactFilter {
	bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) {
		auto entityA = (Entity*) fixtureA->GetBody()->GetUserData();
		auto entityB = (Entity*) fixtureB->GetBody()->GetUserData();

		if (typeid(*entityA) == typeid(Bullet)) {
			std::swap(entityA, entityB);
		}

		if (typeid(*entityA) == typeid(Ship) &&
		    typeid(*entityB) == typeid(Bullet)) {
			auto ship = static_cast<Ship*>(entityA);
			auto bullet = static_cast<Bullet*>(entityB);
			return ship->id != bullet->creator_id;
		}
		
		return true;
	}
} contact_filter;

Game::Game(Scenario &scn, vector<AISourceCode> &ais)
  : ticks(0),
    time(0) {
	int player_ai_index = 0;
	b2Vec2 gravity(0, 0);
	world = std::unique_ptr<b2World>(new b2World(gravity));
	world->SetAutoClearForces(false);
	world->SetContactFilter(&contact_filter);

	for (auto scn_team : scn.teams) {
		auto ai = ais[player_ai_index++];
		auto team = make_shared<Team>(scn_team.name, ai, scn_team.color);
		for (auto scn_ship : scn_team.ships) {
			auto ship = make_shared<Ship>(this, team);
			ship->body->SetTransform(b2Vec2(scn_ship.p.x, scn_ship.p.y), scn_ship.h);
			ship->body->SetLinearVelocity(b2Vec2(scn_ship.v.x, scn_ship.v.y));
			ships.push_back(ship);
		}
	}
}

Game::~Game() {
}

const float tick_length = 1.0f/32;
const int steps_per_tick = 10;
const float step_length = tick_length/steps_per_tick;

static bool entity_is_dead(std::shared_ptr<Entity> entity) {
	return entity->dead;
}

void Game::reap() {
	auto new_end = std::remove_if(begin(bullets), end(bullets), entity_is_dead);	
	bullets.erase(new_end, end(bullets));
}

void Game::tick() {
	reap();

	BOOST_FOREACH(auto ship, ships) {
		ship->tick();
	}

	BOOST_FOREACH(auto bullet, bullets) {
		bullet->tick();
	}

	for (int i = 0; i < steps_per_tick; i++) {
		world->Step(step_length, 8, 3);
	}
	world->ClearForces();
	ticks++;
	time = ticks * tick_length;

#if 0
	auto profile = world->GetProfile();
	printf("\n");
	printf("step: %0.2f ms\n", profile.step);
	printf("collide: %0.2f ms\n", profile.collide);
	printf("solve: %0.2f ms\n", profile.solve);
	printf("solveInit: %0.2f ms\n", profile.solveInit);
	printf("solveVelocity: %0.2f ms\n", profile.solveVelocity);
	printf("solvePosition: %0.2f ms\n", profile.solvePosition);
	printf("broadphase: %0.2f ms\n", profile.broadphase);
	printf("solveTOI: %0.2f ms\n", profile.solveTOI);
#endif
}

shared_ptr<Team> Game::check_victory() {
	std::unordered_set<shared_ptr<Team>> set;
	BOOST_FOREACH(auto ship, ships) {
		if (ship->body->GetPosition().Length() < 30) { // TODO(rlane): set in scenario
			set.insert(ship->team);
		}
	}

	// TODO(rlane): handle size == 0
	if (set.size() == 1) {
		return *set.begin();
	} else {
		return nullptr;
	}
}

}
