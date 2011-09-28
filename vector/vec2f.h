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

#endif
