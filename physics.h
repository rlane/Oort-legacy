#include <complex.h>

#ifndef PHYSICS_H
#define PHYSICS_H

#define VEC2_FMT "(%0.3g, %0.3g)"
typedef complex double vec2;

#define VEC2_ARG(x) creal(x), cimag(x)

#define C(x,y) ((x) + (y)*I)

struct physics {
	vec2 p, p0, v, thrust;
	double a, av;
	double r, m;
};

struct physics *physics_create(void);
void physics_destroy(struct physics *);
void physics_tick(double t);
int physics_check_collision(struct physics *q1, struct physics *q2, double interval, complex double *cp);

double distance(vec2 a, vec2 b);

#endif
