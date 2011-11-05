// Copyright 2011 Rich Lane
#ifndef OORT_SIM_AI_H_
#define OORT_SIM_AI_H_

#include <string>

namespace Oort {

struct AISourceCode {
	std::string filename;
	std::string code;
};

class Ship;

class AI {
public:
	AI(Ship *ship, AISourceCode &src);
	~AI();
	void tick();

private:
	Ship *ship;
};

}

#endif
