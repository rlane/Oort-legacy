// Copyright 2011 Rich Lane
#include "sim/game.h"

#include <memory>
#include <unordered_set>
#include <boost/foreach.hpp>

#include "glm/glm.hpp"

#include "sim/ship.h"

using std::shared_ptr;

namespace Oort {

Game::Game() {
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
