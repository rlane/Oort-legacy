#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

char *data_dir = NULL;

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

gboolean find_data_dir(void)
{
	if (getenv("RISC_DATA")) {
		data_dir = getenv("RISC_DATA");
		return TRUE;
	}

	if (g_file_test("runtime.lua", G_FILE_TEST_EXISTS)) {
		data_dir = ".";
		return TRUE;
	}

#ifdef WIN32
	data_dir = g_win32_get_package_installation_directory_of_module(NULL);
	return data_dir != NULL;
#else
	if (g_file_test("/usr/share/risc", G_FILE_TEST_IS_DIR)) {
		data_dir = "/usr/share/risc";
		return TRUE;
	}

	if (g_file_test("/usr/local/share/risc", G_FILE_TEST_IS_DIR)) {
		data_dir = "/usr/local/share/risc";
		return TRUE;
	}
#endif

	return FALSE;
}

char *data_path(const char *subpath)
{
	int len = strlen(data_dir) + strlen(subpath);
	char *path = malloc(len + 1);
	if (!path) abort();
	sprintf(path, "%s/%s", data_dir, subpath);
	return path;
}
