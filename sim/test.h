// Copyright 2011 Rich Lane
#ifndef OORT_SIM_TEST_H_
#define OORT_SIM_TEST_H_

#include <string>
#include <memory>

namespace Oort {

class Game;

class Test {
public:
	static Test *registered;
	static Test *load(std::string path);

	Test();
	virtual ~Test();
	virtual void after_tick() = 0;
	virtual std::shared_ptr<Game> get_game() = 0;

	bool finished;
};

}

#endif
