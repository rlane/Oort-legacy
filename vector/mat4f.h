#include "vector_types.h"
#include "math3d.h"

#ifndef MAT4F_H
#define MAT4F_H

static inline void mat4f_destroy(Mat4f *m)
{
}

static inline void mat4f_load_identity(Mat4f *m)
{
	m3dLoadIdentity44(m->data);
}

static inline void mat4f_load_scale(Mat4f *m, float x, float y, float z)
{
	m3dScaleMatrix44(m->data, x, y, z);
}

static inline void mat4f_load_translation(Mat4f *m, float x, float y, float z)
{
	m3dTranslationMatrix44(m->data, x, y, z);
}

static inline void mat4f_load_rotation(Mat4f *m, float angle, float x, float y, float z)
{
	m3dRotationMatrix44(m->data, angle, x, y, z);
}

static inline void mat4f_load_ortho(Mat4f *m, float l, float r, float b, float t, float n, float f)
{
	m3dMakeOrthographicMatrix(m->data, l, r, b, t, n, f);
}

static inline void mat4f_load_simple_ortho(Mat4f *m, float x, float y, float aspect, float w)
{
	m3dMakeOrthographicMatrix(m->data,
			                      x-w/2.0f, x+w/2.0f,
			                      y-w*aspect/2.0f, y+w*aspect/2.0f,
														-1.0f, 1.0f);
}

static inline void mat4f_multiply(Mat4f *dest, Mat4f *a, Mat4f *b)
{
	m3dMatrixMultiply44(dest->data, a->data, b->data);
}

static inline void mat4f_invert(Mat4f *dest, Mat4f *m)
{
	m3dInvertMatrix44(dest->data, m->data);
}

#endif
