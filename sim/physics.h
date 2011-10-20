#include <glm/glm.hpp>

#ifndef OORT_SIM_PHYSICS_HPP
#define OORT_SIM_PHYSICS_HPP

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

#endif
