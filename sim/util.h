#include <glib-object.h>

#ifndef _UTIL_H
#define _UTIL_H

long envtol(const char *key, long def);

static inline double rad2deg(double a)
{
	return a * 57.29578;
}

#endif
