#include <stdlib.h>
#include <stdio.h>

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
