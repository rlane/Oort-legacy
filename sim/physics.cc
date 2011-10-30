// Copyright 2011 Rich Lane
#include <sim/physics.h>

#include <stdio.h>

Oort::Physics::Physics() {
	printf("constructing physics\n");
}

Oort::Physics::~Physics() {
	printf("destroying physics\n");
}

void Oort::Physics::tick(double tick_length) {
	p += v * tick_length;
}
