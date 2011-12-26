// Copyright 2011 Rich Lane
#ifndef OORT_GL_CHECK_H_
#define OORT_GL_CHECK_H_

#include <stdio.h>
#include "gl/gl.h"

namespace GL {

inline void check() {
#ifdef __native_client__
#else
	auto err = glGetError();
	if (err != GL_NO_ERROR) {
		fprintf(stderr, "GL error: %s\n", glewGetErrorString(err));
		throw new std::exception();
	}
#endif
}

}

#endif
