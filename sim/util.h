#include <glib-object.h>

#ifndef _UTIL_H
#define _UTIL_H

extern char *data_dir;

long envtol(const char *key, long def);
gboolean find_data_dir(void);
char *data_path(const char *subpath);

static inline double rad2deg(double a)
{
	return a * 57.29578;
}

#endif
