// Copyright 2011 Rich Lane
#ifndef OORT_COMMON_LOG_H_
#define OORT_COMMON_LOG_H_

#include <stdio.h>
#include <stdarg.h>

namespace Oort {

inline void log(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

}

#endif
