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

const struct gfx_class *lookup_gfx_class(const char *name)
{
	if (!(strcmp(name, "fighter"))) return &gfx_fighter;
	if (!(strcmp(name, "mothership"))) return &gfx_mothership;
	if (!(strcmp(name, "missile"))) return &gfx_missile;
	if (!(strcmp(name, "little_missile"))) return &gfx_little_missile;
	return NULL;
}

static void gfx_ship_created(struct ship *s)
{
	s->gfx.class = lookup_gfx_class(s->class->name);
	s->gfx.angle = atan2(s->physics->v.y, s->physics->v.x); // XXX reversed?
}

void init_gl13(void)
{
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "glewInit failed\n");
		abort();
	}

	gfx_ship_create_cb = gfx_ship_created;
}
