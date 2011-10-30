// Copyright 2011 Rich Lane
#include <stdio.h>
#include "sim/ship.h"
#include "sim/physics.h"

namespace Oort {

Ship::Ship() {
	printf("created ship\n");
}

Ship::~Ship() {
	printf("destroyed ship\n");
}

}
