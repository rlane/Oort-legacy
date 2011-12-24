// Copyright 2011 Rich Lane
#ifndef OORT_GL_CHECK_H_
#define OORT_GL_CHECK_H_

#include <stdio.h>
#include <GL/glew.h>

namespace GL {

inline void check() {
	auto err = glGetError();
	if (err != GL_NO_ERROR) {
		fprintf(stderr, "GL error: %s\n", glewGetErrorString(err));
		throw new std::exception();
	}
}

}

#endif
