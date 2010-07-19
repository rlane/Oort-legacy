#include <complex.h>

#ifndef VECTOR_H
#define VECTOR_H

#define VEC2_FMT "(%0.3g, %0.3g)"
typedef complex double vec2;

#define VEC2_ARG(x) creal(x), cimag(x)

#define C(x,y) ((x) + (y)*I)

double distance(vec2 a, vec2 b);

#endif
