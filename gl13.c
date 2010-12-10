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

#include "game.h"
#include "physics.h"
#include "ship.h"
#include "bullet.h"
#include "scenario.h"
#include "team.h"
#include "particle.h"
#include "glutil.h"
#include "renderer.h"

static struct gfx_class gfx_fighter = {
	.rotfactor = 0.5,
};

static struct gfx_class gfx_mothership = {
	.rotfactor = 0.05,
};

static struct gfx_class gfx_missile = {
	.rotfactor = 0.6,
};

static struct gfx_class gfx_little_missile = {
	.rotfactor = 0.7,
};

static struct gfx_class gfx_unknown = {
	.rotfactor = 1.0,
};

struct gfx_class *gfx_fighter_p = &gfx_fighter;
struct gfx_class *gfx_mothership_p = &gfx_mothership;
struct gfx_class *gfx_missile_p = &gfx_missile;
struct gfx_class *gfx_little_missile_p = &gfx_little_missile;
struct gfx_class *gfx_unknown_p = &gfx_unknown;

void init_gl13(void)
{
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "glewInit failed\n");
		abort();
	}
}
