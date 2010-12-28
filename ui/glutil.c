#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <GL/glew.h>

#include "tga.h"

void screenshot(const char *filename)
{
	struct { GLint x, y, width, height; } viewport;
	glGetIntegerv(GL_VIEWPORT, (GLint*) &viewport);

	int fd;
	struct tga_header tga;
	size_t data_size = 3 * viewport.width * viewport.height;
	
	if ((fd = open(filename, O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR)) < 0) {
		perror("open");
		return;
	}

	tga = tga_defaults;
	tga.width = viewport.width;
	tga.height = viewport.height;
	write(fd, &tga, sizeof(tga));

	int i;
	unsigned char *buf = g_malloc(viewport.width*3);
	for (i = 0; i < viewport.height; i++) {
		glReadPixels(0, i, viewport.width, 1, GL_BGR, GL_UNSIGNED_BYTE, buf);
		write(fd, buf, viewport.width*3);
	}
	g_free(buf);

	close(fd);
}
