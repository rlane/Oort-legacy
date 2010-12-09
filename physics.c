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
	q->p = q->p0 = q->v = q->thrust = vec2(0,0);
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
	Vec2 acc = vec2_scale(q->thrust, t/q->m);
	q->p0 = q->p;
	q->p = vec2_add(q->p, vec2_scale(vec2_add(q->v, vec2_scale(acc, 0.5)), t));
	q->v = vec2_add(q->v, acc);
}

double min(double a, double b)
{
	return a < b ? a : b;
}

double collision_time(struct physics *q1, struct physics *q2)
{
	Vec2 dv = vec2_sub(q1->v, q2->v);
	Vec2 dp = vec2_sub(q1->p, q2->p);
	double a = vec2_dot(dv, dv);
	double b = 2*vec2_dot(dp, dv);
	double r_sum = q1->r + q2->r;
	double c = vec2_dot(dp, dp) - r_sum*r_sum;
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

int physics_check_collision(struct physics *q1, struct physics *q2, double interval, Vec2 *rcp)
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
		Vec2 cp = vec2_add(q2->p, vec2_scale(q2->v, t));
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
