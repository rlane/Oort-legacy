// Copyright 2011 Rich Lane
#ifndef OORT_GL_TEXTURE_H_
#define OORT_GL_TEXTURE_H_

#include <vector>
#include "gl/gl.h"
#include "glm/gtc/type_ptr.hpp"

namespace GL {

class Texture {
public:
  GLuint id;

  Texture() {
    glGenTextures(1, &id);
  }

  ~Texture() {
    glDeleteTextures(1, &id);
  }

  void bind(GLenum target=GL_TEXTURE_2D) {
    glBindTexture(target, id);
  }

  static void unbind(GLenum target=GL_TEXTURE_2D) {
    glBindTexture(target, 0);
  }
};

}

#endif
