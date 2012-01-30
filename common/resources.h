// Copyright 2011 Rich Lane
#ifndef OORT_COMMON_RESOURCES_H_
#define OORT_COMMON_RESOURCES_H_

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <boost/foreach.hpp>

#include "common/log.h"

namespace Oort {

#ifdef __native_client__
extern "C" { const char *insrc_lookup(const char*); }
inline std::string load_resource(const std::string &name) {
	//log("reading %s", name.c_str());
	auto data = insrc_lookup(name.c_str());
	if (data == NULL) {
		throw std::runtime_error("resource not found: " + name);
	} else {
		//log("read %s ok", name.c_str());
		return std::string(data);
	}
}
#else

static inline void find_resource(const std::string &name, std::ifstream &file) {
	std::vector<std::string> dirs = {
		".",
	};

	auto srcdir = getenv("srcdir");
	if (srcdir) {
		dirs.push_back(std::string(srcdir));
	}

	auto blddir = getenv("blddir");
	if (blddir) {
		dirs.push_back(std::string(blddir));
	}

	BOOST_FOREACH(auto &dir, dirs) {
		file.open(dir + "/" + name, std::ios::in|std::ios::binary|std::ios::ate);
		if (file) {
			return;
		}
	}

	throw std::runtime_error("resource not found: " + name);
}

inline std::string load_resource(const std::string &name) {
	typedef std::istream_iterator<char> istream_iterator;
	typedef std::ostream_iterator<char> ostream_iterator;
	std::ifstream file;
	find_resource(name, file);
	file.exceptions(std::ifstream::badbit);
	file >> std::noskipws;
	auto size = file.tellg();
	//log("reading %s size %d", name.c_str(), static_cast<int>(size));
	std::string data;
	data.reserve(size);
	file.seekg(0, std::ios::beg);
	std::copy(istream_iterator(file), istream_iterator(),
			      std::back_inserter(data));
	file.close();
	return data;
}
#endif

}

#endif
