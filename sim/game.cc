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
#include "sim/ship_class.h"
#include "sim/bullet.h"
#include "sim/beam.h"
#include "sim/scenario.h"
#include "sim/ai.h"
#include "sim/team.h"
#include "sim/math_util.h"

using std::shared_ptr;
using std::vector;
using std::make_shared;

namespace Oort {

const float tick_length = 1.0f/32;
const int steps_per_tick = 10;
const float step_length = tick_length/steps_per_tick;

class ContactFilter : public b2ContactFilter {
	bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) {
		auto entityA = (Entity*) fixtureA->GetBody()->GetUserData();
		auto entityB = (Entity*) fixtureB->GetBody()->GetUserData();

		if (entityA->is_weapon()) {
			if (entityB->is_weapon()) {
				return false;
			}
			std::swap(entityA, entityB);
		}

		if (entityA->dead || entityB->dead) {
			return false;
		}

		// XXX create Weapon subclass of Entity
		if (typeid(*entityA) == typeid(Ship)) {
			if (typeid(*entityB) == typeid(Bullet)) {
				auto ship = static_cast<Ship*>(entityA);
				auto bullet = static_cast<Bullet*>(entityB);
				return ship->id != bullet->creator_id;
		  } else if (typeid(*entityB) == typeid(Beam)) {
				auto ship = static_cast<Ship*>(entityA);
				auto beam = static_cast<Beam*>(entityB);
				return ship->id != beam->creator_id;
			}
		}
		
		return true;
	}
} contact_filter;

class ContactListener : public b2ContactListener {
	void BeginContact(b2Contact *contact) {
		auto entityA = (Entity*) contact->GetFixtureA()->GetBody()->GetUserData();
		auto entityB = (Entity*) contact->GetFixtureB()->GetBody()->GetUserData();

		if (entityA->dead || entityB->dead) {
			return;
		}

		if (entityA->is_weapon()) {
			std::swap(entityA, entityB);
		}

		if (typeid(*entityA) == typeid(Ship)) {
			auto ship = dynamic_cast<Ship*>(entityA);
			if (typeid(*entityB) == typeid(Bullet)) {
				float dv = glm::length(entityA->get_velocity() - entityB->get_velocity());
				float e = 0.5 * entityB->mass * dv*dv;
				//printf("ship %d; bullet %p; damage %g\n", ship->id, entityB, e);
				ship->hull -= e;
				if (ship->hull < 0) {
					ship->dead = true;
				}
				entityB->dead = true;
			} else if (typeid(*entityB) == typeid(Beam)) {
				auto beam = static_cast<Beam*>(entityB);
				float e = beam->damage * tick_length;
				ship->hull -= e;
				//printf("ship %d; beam %p; damage %g\n", ship->id, entityB, e);
				if (ship->hull < 0) {
					ship->dead = true;
				}
				entityB->dead = true;
			}
		}
	}
} contact_listener;

Game::Game(const Scenario &scn, const vector<std::shared_ptr<AIFactory>> &ai_factories)
  : ticks(0),
    time(0),
    radius(10000) {
	test_finished = false;
	int player_ai_index = 0;
	b2Vec2 gravity(0, 0);
	world = std::unique_ptr<b2World>(new b2World(gravity));
	world->SetAutoClearForces(false);
	world->SetContactFilter(&contact_filter);
	world->SetContactListener(&contact_listener);

	for (auto scn_team : scn.teams) {
		auto ai_factory = ai_factories[player_ai_index++];
		auto team = make_shared<Team>(scn_team.name, ai_factory, scn_team.color);
		for (auto scn_ship : scn_team.ships) {
			auto ship = make_shared<Ship>(this, *fighter, team);
			ship->set_position(scn_ship.p);
			ship->set_heading(scn_ship.h);
			ship->set_velocity(scn_ship.v);
			ships.push_back(ship);
		}
	}
}

Game::~Game() {
}

static bool entity_is_dead(std::shared_ptr<Entity> entity) {
	return entity->dead;
}

void Game::reap() {
	bullets.erase(
		std::remove_if(begin(bullets), end(bullets), entity_is_dead),
		end(bullets));
	beams.clear();
	ships.erase(
		std::remove_if(begin(ships), end(ships), entity_is_dead),
		end(ships));
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
	after_tick();

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

void Game::after_tick() {
}

shared_ptr<Team> Game::check_victory() {
	std::unordered_set<shared_ptr<Team>> set;
	BOOST_FOREACH(auto ship, ships) {
		if (glm::length(ship->get_position()) < radius) { // TODO(rlane): set in scenario
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

shared_ptr<Ship> Game::lookup_ship(uint32 id) {
	BOOST_FOREACH(auto ship, ships) {
		if (ship->id == id) {
			return ship;
		}
	}
	return nullptr;
}

}
