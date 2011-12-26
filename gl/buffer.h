// Copyright 2011 Rich Lane
#ifndef OORT_GL_BUFFER_H_
#define OORT_GL_BUFFER_H_

#include <vector>
#include "gl/gl.h"
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

  void bind(GLenum target=GL_ARRAY_BUFFER) {
    glBindBuffer(target, id);
  }

  static void unbind(GLenum target=GL_ARRAY_BUFFER) {
    glBindBuffer(target, 0);
  }

  template <class T>
  void data(const std::vector<T> &val,
            GLenum target = GL_ARRAY_BUFFER,
            GLenum usage = GL_STATIC_DRAW) {
    bind(target);
    glBufferData(target, val.size()*sizeof(T), &val[0], usage);
    unbind(target);
  }
};

}

#endif
