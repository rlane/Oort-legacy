// Copyright 2011 Rich Lane
#include "sim/game.h"

#include <memory>
#include <vector>
#include <unordered_set>
#include <boost/foreach.hpp>

#include "glm/glm.hpp"
#include <Box2D/Box2D.h>

#include "sim/ship.h"
#include "sim/scenario.h"
#include "sim/ai.h"
#include "sim/team.h"

using std::shared_ptr;
using std::vector;
using std::make_shared;

namespace Oort {

Game::Game(Scenario &scn, vector<AISourceCode> &ais) {
	ticks = 0;
	int player_ai_index = 0;
	b2Vec2 gravity(0, 0);
	world = std::unique_ptr<b2World>(new b2World(gravity));

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

void Game::tick() {
	BOOST_FOREACH(auto ship, ships) {
		ship->tick();
	}
	for (int i = 0; i < steps_per_tick; i++) {
		world->Step(step_length, 8, 3);
	}
	ticks++;

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
