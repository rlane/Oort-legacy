#include <sim/physics.h>

#include <iostream>


using namespace std;

Oort::Physics::Physics() {
	cout << "constructing physics" << endl;
}

Oort::Physics::~Physics() {
	cout << "destroying physics" << endl;
}

void Oort::Physics::tick(double tick_length) {
	p += v * tick_length;
}
