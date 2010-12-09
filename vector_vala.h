#include <math.h>

#ifndef VECTOR_VALA_H
#define VECTOR_VALA_H

typedef struct Vec2 {
	double x;
	double y;
} Vec2;

static Vec2 vector_vec2_scale(Vec2 a, double f)
{
	Vec2 r = { a.x*f, a.y*f };
	return r;
}

static Vec2 vector_vec2_add(Vec2 a, Vec2 b)
{
	Vec2 r = { a.x+b.x, a.y+b.y };
	return r;
}

static Vec2 vector_vec2_sub(Vec2 a, Vec2 b)
{
	Vec2 r = { a.x-b.x, a.y-b.y };
	return r;
}

static double vector_vec2_abs(Vec2 a)
{
	return sqrt(a.x*a.x + a.y*a.y);
}

static Vec2 vector_vec2(double x, double y)
{
	Vec2 r = { x, y };
	return r;
}

#endif
