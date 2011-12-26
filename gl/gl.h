#ifndef GL_GL_H_
#define GL_GL_H_

#ifdef __native_client__
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#define NO_SDL_GLEXT
#include <SDL_opengl.h>
#endif

#endif
