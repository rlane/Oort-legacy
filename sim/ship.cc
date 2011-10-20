#include <iostream>
#include "sim/ship.h"
#include "sim/physics.h"

using namespace std;
using namespace Oort;

Ship::Ship() {
	cout << "created ship" << endl;
}

Ship::~Ship() {
	cout << "destroyed ship" << endl;
}
