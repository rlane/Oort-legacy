#include <complex.h>

#ifndef PHYSICS_H
#define PHYSICS_H

struct physics {
	complex double p, p0, v, thrust;
	double a, av;
	double r, m;
};

struct physics *physics_create(void);
void physics_destroy(struct physics *);
void physics_tick(double t);
int physics_check_collision(struct physics *q1, struct physics *q2, complex double *cp);

#endif
