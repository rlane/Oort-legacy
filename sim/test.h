// Copyright 2011 Rich Lane
#ifndef OORT_SIM_TEST_H_
#define OORT_SIM_TEST_H_

#include <string>
#include <memory>

#include "sim/game.h"

namespace Oort {

class Test : public Game {
public:
	static Test *registered;
	static std::shared_ptr<Test> load(std::string path);

	Test();
	~Test();

	void *dl_handle;
};

}

#endif
