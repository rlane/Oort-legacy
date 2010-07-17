#include <complex.h>

#ifndef PHYSICS_H
#define PHYSICS_H

struct physics {
	complex double p, v, thrust;
	double a, av;
	double r, m;
};

struct physics *physics_create(void);
void physics_destroy(struct physics *);
void physics_tick(double t);

#endif
