// Copyright 2011 Rich Lane
#include "sim/game.h"

#include <memory>
#include <vector>
#include <unordered_set>
#include <typeinfo>
#include <boost/foreach.hpp>
#include <stdexcept>

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
#include "sim/test.h"

using std::shared_ptr;
using std::vector;
using std::make_shared;

namespace Oort {

class ContactFilter : public b2ContactFilter {
	bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) {
		const Entity &entityA = *(Entity*)fixtureA->GetBody()->GetUserData();
		const Entity &entityB = *(Entity*)fixtureB->GetBody()->GetUserData();

		if (entityA.dead || entityB.dead) {
			return false;
		}

		return entityA.should_collide(entityB) &&
		       entityB.should_collide(entityA);
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

		// both weapons or ships
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

	void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
		auto fixtureA = contact->GetFixtureA();
		auto fixtureB = contact->GetFixtureB();
		auto bodyA = fixtureA->GetBody();
		auto bodyB = fixtureB->GetBody();
		auto entityA = (Entity*) bodyA->GetUserData();
		auto entityB = (Entity*) bodyB->GetUserData();
		auto shipA = dynamic_cast<Ship*>(entityA);
		auto shipB = dynamic_cast<Ship*>(entityB);
		if (shipA && shipB) {
			shipA->handle_collision(*shipB);
			shipB->handle_collision(*shipA);
		}
	}
} contact_listener;

Game::Game(const Scenario &scn, const vector<std::shared_ptr<AIFactory>> &ai_factories)
  : ticks(0),
    time(0),
    radius(10000) {
	unsigned int player_ai_index = 0;
	b2Vec2 gravity(0, 0);
#ifdef B2WORLD_OLD_CONSTRUCTOR
	world = std::unique_ptr<b2World>(new b2World(gravity, false));
#else
	world = std::unique_ptr<b2World>(new b2World(gravity));
#endif
	world->SetAutoClearForces(false);
	world->SetContactFilter(&contact_filter);
	world->SetContactListener(&contact_listener);

	BOOST_FOREACH(auto scn_team, scn.teams) {
		if (player_ai_index >= ai_factories.size()) {
			throw std::runtime_error("Not enough AIs given");
		}
		auto ai_factory = ai_factories[player_ai_index++];
		auto team = make_shared<Team>(scn_team.name, ai_factory, scn_team.color);
		BOOST_FOREACH(auto scn_ship, scn_team.ships) {
			ShipClass *klass = NULL;
			if (scn_ship.klass == "fighter") {
				klass = &*fighter;
			} else if (scn_ship.klass == "assault_frigate") {
				klass = &*assault_frigate;
			} else if (scn_ship.klass == "ion_cannon_frigate") {
				klass = &*ion_cannon_frigate;
			} else if (scn_ship.klass == "carrier") {
				klass = &*ion_cannon_frigate; // XXX
			} else if (scn_ship.klass == "missile") {
				klass = &*missile;
			} else if (scn_ship.klass == "target") {
				klass = &*target;
			} else {
				throw std::runtime_error("Unknown ship class");
			}
			auto ship = make_shared<Ship>(this, *klass, team);
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
		std::remove_if(bullets.begin(), bullets.end(), entity_is_dead),
		bullets.end());
	beams.clear();
	ships.erase(
		std::remove_if(ships.begin(), ships.end(), entity_is_dead),
		ships.end());
	hits.clear();
	explosions.clear();
}

void Game::tick() {
	reap();

	Timer ai_timer;
	BOOST_FOREACH(auto ship, ships) {
		ship->tick();
	}
	ai_perf.update(ai_timer);

	BOOST_FOREACH(auto bullet, bullets) {
		bullet->tick();
	}

	Timer physics_timer;
	world->Step(tick_length, 8, 3);
	physics_perf.update(physics_timer);

	BOOST_FOREACH(auto &explosion, explosions) {
		explosion.tick(*this);
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

bool Game::check_victory(Team *&team) {
	team = NULL;

	std::unordered_set<Team*> set;
	BOOST_FOREACH(auto ship, ships) {
		if (glm::length(ship->get_position()) < radius && // TODO(rlane): set in scenario
		    &ship->klass != &*missile) {
			set.insert(ship->team.get());
		}
	}

	if (set.size() == 1) {
		team = *set.begin();
		return true;
	} else if (set.size() == 0) {
		return true;
	} else {
		return false;
	}
}

shared_ptr<Ship> Game::lookup_ship(uint32 id) {
	BOOST_FOREACH(auto ship, ships) {
		if (ship->id == id) {
			return ship;
		}
	}
	return shared_ptr<Ship>();
}

}
