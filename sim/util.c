#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <string.h>
#include <sys/time.h>

#ifdef WITH_VALGRIND
#include <callgrind.h>
#endif

long envtol(const char *key, long def)
{
	const char *value = getenv(key);

	if (!value) {
		return def;
	}

	char *endptr;
	long l = strtol(value, &endptr, 10);

	if (endptr == value || *endptr) {
		fprintf(stderr, "invalid value for %s, defaulting to %ld\n", key, def);
		return def;
	}

	return l;
}

void *leak(void *arg)
{
	return arg;
}

guint64 thread_ns(void)
{
#ifdef CLOCK_THREAD_CPUTIME_ID
	struct timespec ts;
	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts)) {
		perror("glock_gettime");
		abort();
	}
	return ts.tv_nsec + ts.tv_sec*(1000*1000*1000);
#else
	return 0;
#endif
}

void risc_util_toggle_callgrind_collection() {
#ifdef WITH_VALGRIND
	CALLGRIND_TOGGLE_COLLECT;
#endif
}
