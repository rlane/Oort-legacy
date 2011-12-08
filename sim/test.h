// Copyright 2011 Rich Lane
#ifndef OORT_SIM_TEST_H_
#define OORT_SIM_TEST_H_

#include <string>
#include <memory>

namespace Oort {

class Game;
typedef void (*test_cb)(Game *);
typedef Game *(*test_init_cb)();

class Test {
public:
	std::string path;
	std::shared_ptr<Game> game;

	Test(std::string path);
	~Test();

	void hook(const char *key);

private:
	void *dl_handle;
};

}

#endif
