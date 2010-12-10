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
int paused;
int single_step;
int render_all_debug_lines;
struct ship *picked;

static GRand *gfx_prng;

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

static void render_particles(void)
{
	int i;
	for (i = 0; i < MAX_PARTICLES; i++) {
		struct particle *c = &particles[i];
		if (c->ticks_left == 0) continue;
		Vec2 p = S(c->p);
		if (c->type == PARTICLE_HIT) {
			glPointSize(0.3*c->ticks_left*view_scale/32);
			glColor4ub(255, 200, 200, c->ticks_left*8);
		} else if (c->type == PARTICLE_PLASMA) {
			glPointSize(0.15*c->ticks_left*view_scale/32);
			glColor4ub(255, 0, 0, c->ticks_left*32);
		} else if (c->type == PARTICLE_ENGINE) {
			glPointSize(0.1*c->ticks_left*view_scale/32);
			glColor4ub(255, 217, 43, 10 + c->ticks_left*5);
		}
		glBegin(GL_POINTS);
		glVertex3f(p.x, p.y, 0);
		glEnd();
	}
}

void render_gl13(int _paused, int _render_all_debug_lines)
{
	g_rand_set_seed(gfx_prng, ticks);
	paused = _paused;
	render_all_debug_lines = _render_all_debug_lines;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	render_particles();

	if (picked) {
		const int x = 15, y = 82, dy = 12;
		glColor32(0xAAFFFFAA);
		glPrintf(x, y-0*dy, "%s %.8x", picked->class->name, picked->api_id);
		glPrintf(x, y-1*dy, "hull: %.2f", picked->hull);
		glPrintf(x, y-2*dy, "position: " VEC2_FMT, VEC2_ARG(picked->physics->p));
		glPrintf(x, y-3*dy, "velocity: " VEC2_FMT " %g", VEC2_ARG(picked->physics->v), vec2_abs(picked->physics->v));
		glPrintf(x, y-4*dy, "thrust: " VEC2_FMT " %g", VEC2_ARG(picked->physics->thrust), vec2_abs(picked->physics->thrust));
		glPrintf(x, y-5*dy, "energy: %g", ship_get_energy(picked));
	}
}

static double normalize_angle(double a)
{
	if (a < -M_PI) a += 2*M_PI;
	if (a > M_PI) a -= 2*M_PI;
	return a;
}

static void emit_ship(struct ship *s, void *unused)
{
	if (vec2_abs(s->physics->thrust) != 0) {
		particle_shower(PARTICLE_ENGINE, s->physics->p, vec2_scale(s->physics->v, 1/32), vec2_scale(s->physics->thrust, -1/32), 0.1, 1, 4, 8);
	}

	if (s->gfx.class) {
		double v_angle = atan2(s->physics->v.y, s->physics->v.x); // XXX reversed?
		double da = normalize_angle(v_angle - s->gfx.angle);
		s->gfx.angle = normalize_angle(s->gfx.angle + s->gfx.class->rotfactor*da);
	}
}

void emit_particles(void)
{
	g_list_foreach(all_ships, (GFunc)emit_ship, NULL);
}

void init_gl13(void)
{
	gfx_prng = g_rand_new();

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
	paused = 0;
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
