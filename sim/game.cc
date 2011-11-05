// Copyright 2011 Rich Lane
#include "sim/game.h"

#include <memory>
#include <vector>
#include <unordered_set>
#include <boost/foreach.hpp>

#include "glm/glm.hpp"

#include "sim/ship.h"
#include "sim/scenario.h"
#include "sim/ai.h"

using std::shared_ptr;
using std::vector;
using std::make_shared;

namespace Oort {

Game::Game(Scenario &scn, vector<AISourceCode> &ais) {
	int player_ai_index = 0;

	for (auto scn_team : scn.teams) {
		auto ai = ais[player_ai_index++];
		auto team = make_shared<Team>(scn_team.name, ai, scn_team.color);
		for (auto scn_ship : scn_team.ships) {
			auto ship = make_shared<Ship>(team);
			ship->physics.p = scn_ship.p;
			ship->physics.v = scn_ship.v;
			ship->physics.h = scn_ship.h;
			ships.push_back(ship);
		}
	}
}

Game::~Game() {
}

void Game::tick() {
	BOOST_FOREACH(auto ship, ships) {
		ship->tick();
		ship->physics.tick(1.0/32);
	}
}

shared_ptr<Team> Game::check_victory() {
	std::unordered_set<shared_ptr<Team>> set;
	BOOST_FOREACH(auto ship, ships) {
		if (glm::length(ship->physics.p) < 30) { // TODO(rlane): set in scenario
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
