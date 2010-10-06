#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <glib.h>

#include "physics.h"

GList *all_physics = NULL;
static GList *new_physics = NULL;
static GStaticMutex new_physics_lock = G_STATIC_MUTEX_INIT;

struct physics *physics_create(void)
{
	struct physics *q = g_slice_new(struct physics);
	q->p = q->p0 = q->v = q->thrust = 0.0 + 0.0*I;
	q->a = q->av = 0.0;
	q->m = q->r = 1.0;
	g_static_mutex_lock(&new_physics_lock);
	new_physics = g_list_append(new_physics, q);
	g_static_mutex_unlock(&new_physics_lock);
	return q;
}

void physics_destroy(struct physics *q)
{
	all_physics = g_list_remove(all_physics, q);
	g_slice_free(struct physics, q);
}

void physics_tick_one(struct physics *q, const double *ta)
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

double dot(complex double a, complex double b)
{
	return creal(a)*creal(b) + cimag(a)*cimag(b);
}

double min(double a, double b)
{
	return a < b ? a : b;
}

double collision_time(struct physics *q1, struct physics *q2)
{
	vec2 dv = q1->v - q2->v;
	vec2 dp = q1->p - q2->p;
	double a = dot(dv, dv);
	double b = 2*dot(dp, dv);
	double r_sum = q1->r + q2->r;
	double c = dot(dp, dp) - r_sum*r_sum;
	double disc = b*b - 4*a*c;
	if (disc < 0) {
		return NAN;
	} else if (disc == 0) {
		return -b/(2*a);
	} else {
		double t0 = (-b - sqrt(disc))/(2*a);
		double t1 = (-b + sqrt(disc))/(2*a);
		return min(t0, t1);
	}
}

int physics_check_collision(struct physics *q1, struct physics *q2, double interval, complex double *rcp)
{
	double t = collision_time(q1, q2);
	if (isnan(t)) {
		return 0;
	} if (t < 0) {
		// past collision
		return 0;
	} if (t > interval) {
		// future collision
		return 0;
	} else {
		vec2 cp = q2->p + t*q2->v;
		if (rcp) *rcp = cp;
		return 1;
	}
}

void physics_tick(double t)
{
	all_physics = g_list_concat(all_physics, new_physics);
	new_physics = NULL;
	g_list_foreach(all_physics, (GFunc)physics_tick_one, &t);
}
