// Copyright 2011 Rich Lane
#include <glm/glm.hpp>

#ifndef OORT_SIM_PHYSICS_H_
#define OORT_SIM_PHYSICS_H_

namespace Oort {

class Physics {
	public:
	glm::dvec2 p, v, a;
	double h, w, wa;
	double r;
	double m;

	Physics();
	~Physics();

	void tick(double tick_length);
};

}

#endif  // OORT_SIM_PHYSICS_H
