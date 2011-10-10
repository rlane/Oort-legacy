#include <math.h>
#include <math3d.h>
#include "vector_types.h"

#ifndef VEC2F_H
#define VEC2F_H

static inline Vec2f vec2f(float x, float y)
{
	Vec2f r = {{ x, y }};
	return r;
}

static inline Vec2f vec2f_scale(Vec2f a, float f)
{
	return vec2f(a.x*f, a.y*f);
}

static inline Vec2f vec2f_add(Vec2f a, Vec2f b)
{
	return vec2f(a.x+b.x, a.y+b.y);
}

#endif
