#include "vector.h"

#ifndef PHYSICS_H
#define PHYSICS_H

struct physics {
	Vec2 p, p0, v, thrust;
	double a, av;
	double r, m;
};

struct physics *physics_create(void);
void physics_destroy(struct physics *);
void physics_tick(double t);
void physics_tick_one(struct physics *q, const double *ta);
int physics_check_collision(struct physics *q1, struct physics *q2, double interval, Vec2 *cp);

static inline double rad2deg(double a)
{
	return a * 57.29578;
}

static inline struct physics *risc_physics_new()
{
	return g_malloc(sizeof(struct physics));
}

static inline void risc_physics_free(struct physics *q)
{
	g_free(q);
}

#endif
