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

const double zoom_force = 0.1;
const double min_view_scale = 6;
const double max_view_scale = 150;

int screen_width = 640;
int screen_height = 480;

Vec2 view_pos;
double view_scale;
int single_step;
int render_all_debug_lines;
struct ship *picked;

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

Vec2 S(Vec2 p)
{
	return vec2_add(vec2_scale(vec2_sub(p, view_pos), view_scale),
		              vec2(screen_width/2, screen_height/2));
}

void render_gl13(int _paused, int _render_all_debug_lines)
{
	render_all_debug_lines = _render_all_debug_lines;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
}

void init_gl13(void)
{
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "glewInit failed\n");
		abort();
	}

	gfx_ship_create_cb = gfx_ship_created;
}

void reshape_gl13(int width, int height)
{
	screen_width = width;
	screen_height = height;
}

void reset_gl13()
{
	view_pos = vec2(0,0);
	view_scale = 16.0;
	single_step = 0;
	render_all_debug_lines = 0;
	picked = NULL;
}

static Vec2 W(Vec2 o)
{
	return vec2_add(view_pos,
			            vec2_scale(vec2_sub(o,
											                vec2(screen_width/2,
																				   screen_height/2)),
										         1/view_scale));
}

void pick(int x, int y)
{
	Vec2 p = W(vec2(x, y));
	GList *es;
	for (es = g_list_first(all_ships); es; es = g_list_next(es)) {
		struct ship *s = es->data;
		if (vec2_distance(s->physics->p, p) < s->physics->r) {
			picked = s;
			return;
		}
	}
	picked = NULL;
}

void zoom(int x, int y, double f)
{
	if (view_scale != min_view_scale && view_scale != max_view_scale) {
		view_pos = vec2_add(vec2_scale(view_pos, 1-zoom_force), vec2_scale(W(vec2(x,y)), zoom_force));
	}
	view_scale *= f;
	view_scale = MIN(MAX(view_scale, min_view_scale), max_view_scale);
}
