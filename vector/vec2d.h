#include <math.h>
#include "vector_types.h"

#ifndef VEC2D_H
#define VEC2D_H

static inline Vec2 vec2(double x, double y)
{
	Vec2 r = {{ x, y }};
	return r;
}

static inline Vec2f vec2_to_vec2f(Vec2 v)
{
	Vec2f r = {{ (float)v.x, (float)v.y }};
	return r;
}

static inline Vec2 vec2_scale(Vec2 a, double f)
{
	return vec2(a.x*f, a.y*f);
}

static inline Vec2 vec2_add(Vec2 a, Vec2 b)
{
	return vec2(a.x+b.x, a.y+b.y);
}

static inline Vec2 vec2_sub(Vec2 a, Vec2 b)
{
	return vec2(a.x-b.x, a.y-b.y);
}

static inline double vec2_abs(Vec2 a)
{
	return sqrt(a.x*a.x + a.y*a.y);
}

static inline double vec2_distance(Vec2 a, Vec2 b)
{
	return vec2_abs(vec2_sub(a, b));
}

static inline double vec2_dot(Vec2 a, Vec2 b)
{
	return a.x*b.x + a.y*b.y;
}

static inline Vec2 vec2_rotate(Vec2 a, double angle)
{
	return vec2(a.x*cos(angle) - a.y*sin(angle),
		          a.x*sin(angle) + a.y*cos(angle));
}

#endif
