#include <math.h>

#ifndef VECTOR_H
#define VECTOR_H

#define VEC2_FMT "(%0.3g, %0.3g)"

#define VEC2_ARG(v) v.x, v.y

typedef struct Vec2 {
	double x;
	double y;
} Vec2;

static inline Vec2 vec2_scale(Vec2 a, double f)
{
	Vec2 r = { a.x*f, a.y*f };
	return r;
}

static inline Vec2 vec2_add(Vec2 a, Vec2 b)
{
	Vec2 r = { a.x+b.x, a.y+b.y };
	return r;
}

static inline Vec2 vec2_sub(Vec2 a, Vec2 b)
{
	Vec2 r = { a.x-b.x, a.y-b.y };
	return r;
}

static inline double vec2_abs(Vec2 a)
{
	return sqrt(a.x*a.x + a.y*a.y);
}

static inline Vec2 vec2(double x, double y)
{
	Vec2 r = { x, y };
	return r;
}

static inline double vec2_distance(Vec2 a, Vec2 b)
{
	return vec2_abs(vec2_sub(a, b));
}

static inline double vec2_dot(Vec2 a, Vec2 b)
{
	return a.x*b.x + a.y*b.y;
}

#endif
