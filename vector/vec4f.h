#include <math.h>

#ifndef VEC4F_H
#define VEC4F_H

typedef struct Vec4f {
	float x;
	float y;
	float z;
	float w;
} Vec4f;

static inline Vec4f vec4f_scale(Vec4f a, float f)
{
	Vec4f r = { a.x*f, a.y*f, a.z*f, a.w*f };
	return r;
}

static inline Vec4f vec4f_add(Vec4f a, Vec4f b)
{
	Vec4f r = { a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w };
	return r;
}

static inline Vec4f vec4f_sub(Vec4f a, Vec4f b)
{
	Vec4f r = { a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w };
	return r;
}

static inline float vec4f_abs(Vec4f a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
}

static inline Vec4f vec4f(float x, float y, float z, float w)
{
	Vec4f r = { x, y, z, w };
	return r;
}

static inline float vec4f_distance(Vec4f a, Vec4f b)
{
	return vec4f_abs(vec4f_sub(a, b));
}

static inline float vec4f_dot(Vec4f a, Vec4f b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

#endif
