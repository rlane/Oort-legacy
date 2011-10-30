// Copyright 2011 Rich Lane
#include "sim/ship.h"
#include "sim/physics.h"
#include "common/log.h"

namespace Oort {

Ship::Ship() {
	log("created ship\n");
}

Ship::~Ship() {
	log("destroyed ship\n");
}

}
