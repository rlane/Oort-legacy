// Copyright 2011 Rich Lane
#ifndef OORT_COMMON_RESOURCES_H_
#define OORT_COMMON_RESOURCES_H_

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>

#include "common/log.h"

namespace Oort {

inline std::string load_resource(std::string name) {
	typedef std::istream_iterator<char> istream_iterator;
	typedef std::ostream_iterator<char> ostream_iterator;
	std::ifstream file;
	file.exceptions(std::ifstream::badbit);
	// TODO(rlane): resources directory
	file.open(name, std::ios::in|std::ios::binary|std::ios::ate);
	file >> std::noskipws;
	auto size = file.tellg();
	log("reading %s size %d\n", name.c_str(), static_cast<int>(size));
	std::string data;
	data.reserve(size);
	file.seekg(0, std::ios::beg);
	std::copy(istream_iterator(file), istream_iterator(),
			      std::back_inserter(data));
	file.close();
	return data;
}

}

#endif
