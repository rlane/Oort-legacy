#include <stdarg.h>

#ifndef GLUTIL_H
#define GLUTIL_H

#ifdef __native_client__
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif

void screenshot(const char *filename);

void glCheck(void);

#endif
