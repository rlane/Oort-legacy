#include <math.h>
#include <math3d.h>
#include "vector_types.h"

#ifndef VEC4F_H
#define VEC4F_H

static inline Vec4f vec4f(float x, float y, float z, float w)
{
	Vec4f r = {{ x, y, z, w }};
	return r;
}

static inline Vec4f vec4f_scale(Vec4f a, float f)
{
	return vec4f(a.x*f, a.y*f, a.z*f, a.w*f);
}

static inline Vec4f vec4f_add(Vec4f a, Vec4f b)
{
	return vec4f(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
}

static inline Vec4f vec4f_sub(Vec4f a, Vec4f b)
{
	return vec4f(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
}

static inline float vec4f_abs(Vec4f a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
}

static inline float vec4f_distance(Vec4f a, Vec4f b)
{
	return vec4f_abs(vec4f_sub(a, b));
}

static inline float vec4f_dot(Vec4f a, Vec4f b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

static inline Vec4f vec4f_transform(Vec4f v, Mat4f *m)
{
	Vec4f r;
	m3dTransformVector4(r.data, v.data, m->data);
	return r;
}

static inline Vec4f vec4f_projectXY(Vec4f v, Mat4f *mModelView, Mat4f *mProjection, int viewport[4])
{
	Vec4f r = vec4f(0, 0, 0, 0);
	m3dProjectXY(r.data, mModelView->data, mProjection->data, viewport, v.data);
	return r;
}

#endif
