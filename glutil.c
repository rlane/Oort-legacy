#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <complex.h>
#include <sys/time.h>
#include <math.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef WINDOWS
#include <sys/mman.h>
#endif

#define GL_GLEXT_PROTOTYPES
#include <SDL.h>
#include <SDL_gfxPrimitives_font.h>
#include <SDL_opengl.h>

#include "tga.h"
#include "renderer.h"

static GLubyte font[256*8];

void font_init(void)
{
	int i, j;
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 8; j++) {
			font[i*8 + j] = gfxPrimitivesFontdata[i*8 + (7-j)];
		}
	}
}

void glWrite(int x, int y, const char *str)
{
#ifndef WINDOWS
	glWindowPos2i(x, y);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

	char c;
	while ((c = *str++)) {
		glBitmap(8, 8, 4, 4, 9, 0, font + 8*c);
	}
#endif
}

void glPrintf(int x, int y, const char *fmt, ...)
{
	static char buf[1024];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	glWrite(x, y, buf);
}

void screenshot(const char *filename)
{
#ifndef WINDOWS
	int fd;
	struct tga_header *tga;
	size_t data_size = 3 * screen_width * screen_height;
	size_t map_size = (sizeof(*tga) + data_size + 4095) & (~0 << 12);
	
	if ((fd = open(filename, O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR)) < 0) {
		perror("open");
		return;
	}

	lseek(fd, sizeof(*tga) + data_size - 1, SEEK_SET);
	write(fd, "\x00", 1);

	if ((tga = mmap(NULL, map_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return;
	}

	*tga = tga_defaults;
	tga->width = screen_width;
	tga->height = screen_height;

	glReadPixels(0, 0, screen_width, screen_height, GL_BGR, GL_UNSIGNED_BYTE, tga->data);

	munmap(tga, map_size);
	close(fd);
#endif
}
