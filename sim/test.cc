// Copyright 2011 Rich Lane
#include "sim/test.h"

#include <dlfcn.h>
#include <stdio.h>
#include <exception>

#include "sim/game.h"
#include "sim/scenario.h"
#include "sim/ai.h"

namespace Oort {

Test *Test::registered;

Test *Test::load(std::string path) {
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

	if (Test::registered == NULL) {
		//throw std::runtime_error("no test registered");
		fprintf(stderr, "no test registered\n");
		exit(1);
	}

	auto tmp = Test::registered;
	Test::registered = NULL;
	return tmp;
}

Test::Test() {
	registered = this;
}

Test::~Test() {
}

}
