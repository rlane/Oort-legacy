// Copyright 2011 Rich Lane
#include "sim/physics.h"
#include "common/log.h"

Oort::Physics::Physics() {
	//log("constructing physics\n");
}

Oort::Physics::~Physics() {
	//log("destroying physics\n");
}

void Oort::Physics::tick(double tick_length) {
	// TODO(rlane): use midpoint approximation
	p += v * tick_length;
	v += a * tick_length;
}
