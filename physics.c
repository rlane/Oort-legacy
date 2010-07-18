#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <glib.h>

#include "physics.h"

GList *all_physics = NULL;

struct physics *physics_create(void)
{
	struct physics *q = g_slice_new(struct physics);
	q->p = q->p0 = q->v = q->thrust = 0.0 + 0.0*I;
	q->a = q->av = 0.0;
	q->m = q->r = 1.0;
	all_physics = g_list_append(all_physics, q);
	return q;
}

void physics_destroy(struct physics *q)
{
	all_physics = g_list_remove(all_physics, q);
	g_slice_free(struct physics, q);
}

void physics_tick_one(struct physics *q, double *ta)
{
	double t = *ta;
	complex double acc = q->thrust*t/q->m;
	q->p0 = q->p;
	q->p += (q->v + acc/2)*t;
	q->v += acc;
}

double distance(complex double a, complex double b)
{
	double dx = creal(a) - creal(b);
	double dy = cimag(a) - cimag(b);
	return sqrt(dx*dx + dy*dy);
}

int physics_check_collision(struct physics *q1, struct physics *q2, complex double *rcp)
{
	double r = q1->r + q2->r;
	if (distance(q1->p0, q2->p0) <= r) {
		*rcp = q2->p0;
		return 1;
	}

	if (distance(q1->p, q2->p) <= r) {
		*rcp = q2->p;
		return 1;
	}

	complex double dp0 = q2->p0 - q1->p0;
	complex double dp = q2->p - q1->p;
	complex double l = dp - dp0;
	double lx = creal(l);
	double ly = cimag(l);
	double dx = creal(dp0);
	double dy = cimag(dp0);
	double u = -(dx*lx + dy*ly)/(dx*dx + dy*dy);
	complex double cp = dp0 + u*l;

#if 0
	printf("cp.x=%0.2g cp.y=%0.2g dist=%0.3g\n", creal(cp), cimag(cp), distance(cp, 0));
#endif

	*rcp = q2->p0 + u*l;
	return u >= 0 && u <= 1 && distance(cp, 0) <= r;
}

void physics_tick(double t)
{
	g_list_foreach(all_physics, (GFunc)physics_tick_one, &t);
}
