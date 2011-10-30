// Copyright 2011 Rich Lane
#ifndef OORT_GL_BUFFER_H_
#define OORT_GL_BUFFER_H_

#include <GL/glew.h>
#include "glm/gtc/type_ptr.hpp"

namespace GL {

class Buffer {
public:
  GLuint id;

  Buffer() {
    glGenBuffers(1, &id);
  }

  ~Buffer() {
    glDeleteBuffers(1, &id);
  }
};

}

#endif
