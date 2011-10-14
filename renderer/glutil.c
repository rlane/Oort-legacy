#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "glutil.h"

#if defined(WIN32)
#include <GL/wglew.h>
#elif !defined(__native_client__)
#include <GL/glxew.h>
#include <GL/glx.h>
#endif

#include "tga.h"

void screenshot(const char *filename)
{
	struct { GLint x, y, width, height; } viewport;
	glGetIntegerv(GL_VIEWPORT, (GLint*) &viewport);

	int fd;
	struct tga_header tga;
	
	if ((fd = open(filename, O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR)) < 0) {
		perror("open");
		return;
	}

	tga = tga_defaults;
	tga.width = viewport.width;
	tga.height = viewport.height;
	write(fd, &tga, sizeof(tga));

	int i, j;
	unsigned char *buf = g_malloc(viewport.width*3);
	for (i = 0; i < viewport.height; i++) {
		glReadPixels(0, i, viewport.width, 1, GL_RGB, GL_UNSIGNED_BYTE, buf);
		for (j = 0; j < viewport.width; j++) {
			unsigned char tmp = buf[3*j];
			buf[3*j] = buf[3*j+2];
			buf[3*j+2] = tmp;
		}
		write(fd, buf, viewport.width*3);
	}
	g_free(buf);

	close(fd);
}

void glCheck(void)
{
#ifdef __native_client__
#else
	int err = glGetError();
	if (err != GL_NO_ERROR) {
		g_error("%s", glewGetErrorString(err));
	}
#endif
}

static void enable_vsync(void)
{
#if defined(WIN32)
	wglSwapIntervalEXT(1);
#elif !defined(__native_client__)
	if (0) {
#ifdef GLX_SGI_swap_control
	} else if (glXSwapIntervalSGI != NULL) {
		glXSwapIntervalSGI(1);
#endif
#ifdef GLX_MESA_swap_control
	} else if (glXSwapIntervalMESA != NULL) {
		glXSwapIntervalMESA(1);
#endif
	} else {
		printf("vsync not supported\n");
	}
#endif
}

void gl_platform_init(void)
{
#ifndef __native_client__
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
#endif
	enable_vsync();
}
