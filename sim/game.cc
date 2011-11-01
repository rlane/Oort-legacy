// Copyright 2011 Rich Lane
#include "sim/game.h"

#include <boost/foreach.hpp>

#include "sim/ship.h"

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

}
