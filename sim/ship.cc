// Copyright 2011 Rich Lane
#include "sim/ship.h"
#include "sim/physics.h"
#include "common/log.h"

namespace Oort {

Ship::Ship(std::shared_ptr<Team> team)
	: team(team)	{
	//log("created ship\n");
}

Ship::~Ship() {
	//log("destroyed ship\n");
}

}
