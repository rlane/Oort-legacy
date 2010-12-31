#include <glib-object.h>

#ifndef _UTIL_H
#define _UTIL_H

long envtol(const char *key, long def);
guint64 thread_ns(void);

static inline double rad2deg(double a)
{
	return a * 57.29578;
}

static inline int is_win32(void)
{
#ifdef G_OS_WIN32
	return 1;
#else
	return 0;
#endif
}

#endif
