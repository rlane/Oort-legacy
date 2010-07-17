#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <sys/time.h>
#include <glib.h>

#include "physics.h"

GList *all_physics = NULL;

struct physics *physics_create(void)
{
	struct physics *q = g_slice_new(struct physics);
	q->p = q->v = q->thrust = 0.0 + 0.0*I;
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
	q->p += (q->v + acc/2)*t;
	q->v += acc;
}

void physics_tick(double t)
{
	g_list_foreach(all_physics, (GFunc)physics_tick_one, &t);
}
