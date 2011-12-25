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

		if (typeid(*entityA) == typeid(Ship) && entityA->get_id() == entityB->creator_id ||
				typeid(*entityB) == typeid(Ship) && entityB->get_id() == entityA->creator_id) {
			return false;
		}
		
		return true;
	}
} contact_filter;

class ContactListener : public b2ContactListener {
	void BeginContact(b2Contact *contact) {
		auto fixtureA = contact->GetFixtureA();
		auto fixtureB = contact->GetFixtureB();
		auto bodyA = fixtureA->GetBody();
		auto bodyB = fixtureB->GetBody();
		auto entityA = (Entity*) bodyA->GetUserData();
		auto entityB = (Entity*) bodyB->GetUserData();

		if (entityA->dead || entityB->dead) {
			return;
		}

		bool weapA = entityA->is_weapon();
		bool weapB = entityB->is_weapon();

		if (weapA) {
			std::swap(entityA, entityB);
			std::swap(weapA, weapB);
		}

		if (weapA || !weapB) {
			return;
		}

		assert(typeid(*entityA) == typeid(Ship));
		assert(entityB->is_weapon());

		auto ship = dynamic_cast<Ship*>(entityA);
		auto weapon = dynamic_cast<Weapon*>(entityB);
		auto xfA = bodyA->GetTransform();
		auto xfB = bodyB->GetTransform();
		auto shapeA = fixtureA->GetShape();
		auto shapeB = fixtureB->GetShape();

		auto &game = *ship->game;
		b2Manifold m;
		contact->Evaluate(&m, xfA, xfB);
		if (m.pointCount > 0) {
			b2WorldManifold worldManifold;
			worldManifold.Initialize(&m, xfA, shapeA->m_radius, xfB, shapeB->m_radius);
			auto cp = b2n(worldManifold.points[0]);
			game.hits.emplace_back(Hit{ ship, weapon, cp, weapon->damage(*ship) });
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
	hits.clear();
	explosions.clear();
}

void Game::tick() {
	reap();

	BOOST_FOREACH(auto ship, ships) {
		ship->tick();
	}

	BOOST_FOREACH(auto bullet, bullets) {
		bullet->tick();
	}

	BOOST_FOREACH(auto &explosion, explosions) {
		explosion.tick(*this);
	}

	for (int i = 0; i < steps_per_tick; i++) {
		world->Step(step_length, 8, 3);
	}

	world->ClearForces();

	BOOST_FOREACH(auto &hit, hits) {
		hit.ship->hull -= hit.e;
		if (hit.ship->hull < 0) {
			hit.ship->dead = true;
		}
		if (hit.weapon) {
			hit.weapon->dead = true;
		}
	}

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

	if (set.size() == 1) {
		return *set.begin();
	} else if (set.size() == 0) {
		// TODO(rlane): handle size == 0
		printf("tie\n");
		abort();
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
