// Copyright 2011 Rich Lane
#ifndef OORT_SIM_AI_H_
#define OORT_SIM_AI_H_

#include <string>

namespace Oort {

class AI {
public:
	std::string filename;
	std::string code;

	AI(std::string filename, std::string code)
		: filename(filename), code(code) {}
};

}

#endif
