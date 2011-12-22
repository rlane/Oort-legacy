// Copyright 2011 Rich Lane
#include "sim/test.h"

#include <dlfcn.h>
#include <exception>

#include "sim/game.h"
#include "sim/scenario.h"
#include "sim/ai.h"

namespace Oort {

Test *Test::registered;

static void dl_deleter(Test *ptr) {
	dlclose(ptr->dl_handle);
}

std::shared_ptr<Test> Test::load(std::string path) {
	assert(registered == NULL);
	char *p = realpath(path.c_str(), NULL);
	if (p == NULL) {
		fprintf(stderr, "test not found\n");
		exit(1);
	}

	auto dl_handle = dlopen(p, RTLD_NOW|RTLD_LOCAL);
	free(p);

	if (dl_handle == NULL) {
		fprintf(stderr, "dlopen: %s\n", dlerror());
		exit(1);
	}

	if (registered == NULL) {
		//throw std::runtime_error("no test registered");
		fprintf(stderr, "no test registered\n");
		exit(1);
	}

	registered->dl_handle = dl_handle;
	auto tmp = registered;
	registered = NULL;
	return std::shared_ptr<Test>(tmp, dl_deleter);
}

Test::Test()
  : Game(Scenario(), std::vector<std::shared_ptr<AIFactory>>()) {
	registered = this;
}

Test::~Test() {
}

}
