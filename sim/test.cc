// Copyright 2011 Rich Lane
#include "sim/test.h"

#include <dlfcn.h>

#include "sim/game.h"

namespace Oort {

Test::Test(std::string path)
  : path(path) {

	dl_handle = dlopen(path.c_str(), RTLD_NOW|RTLD_LOCAL);
	if (dl_handle == NULL) {
		fprintf(stderr, "dlopen: %s", dlerror());
		exit(1);
	}

	auto cb = (test_init_cb) dlsym(dl_handle, "test_init");
	if (cb == NULL) {
		fprintf(stderr, "could not find test_init\n");
		exit(1);
	}
	game = std::shared_ptr<Game>(cb());
}

void Test::hook(const char *key) {
	char buf[128];

	snprintf(buf, sizeof(buf), "test_%s", key);

	auto cb = (test_cb) dlsym(dl_handle, buf);
	if (cb == NULL) {
		fprintf(stderr, "could not find hook function %s\n", buf);
		exit(1);
	}

	cb(game.get());
}

Test::~Test() {
	if (dl_handle != NULL) {
		dlclose(dl_handle);
	}
	fprintf(stderr, "test finished\n");
}

}
