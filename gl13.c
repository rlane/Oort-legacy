#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <complex.h>
#include <sys/time.h>
#include <math.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <GL/glew.h>

#include "renderer.h"

void init_gl13(void)
{
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "glewInit failed\n");
		abort();
	}
}
