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

static Vec2 S(Vec2 p)
{
	return vec2_add(vec2_scale(vec2_sub(p, view_pos), view_scale),
		              vec2(screen_width/2, screen_height/2));
}

static void render_circle(int n)
{
	double da = 2*M_PI/n, a = 0;
	int i;

	glBegin(GL_LINE_LOOP);
	for (i = 0; i < n; i++) {
		a += da;
		glVertex3f(cos(a), sin(a), 0);
	}
	glEnd();
}

static void triangle_fractal(int depth)
{
	const double alt = 0.8660254;

	if (depth > 1) {
		glBegin(GL_LINES);
		glVertex3f(alt, 0, 0);
		glVertex3f(3*alt/4, -0.125, 0);
		glVertex3f(alt/4, -0.375, 0);
		glVertex3f(0, -0.5, 0);
		glVertex3f(alt, 0, 0);
		glVertex3f(3*alt/4, 0.125, 0);
		glVertex3f(alt/4, 0.375, 0);
		glVertex3f(0, 0.5, 0);
		glEnd();

		glPushMatrix();
		glScaled(0.5, 0.5, 0.5);
		glRotated(60, 0, 0, 1);
		glTranslated(alt, -0.5, 0);
		triangle_fractal(depth-1);
		glPopMatrix();

		glPushMatrix();
		glScaled(0.5, 0.5, 0.5);
		glRotated(-60, 0, 0, 1);
		glTranslated(alt, 0.5, 0);
		triangle_fractal(depth-1);
		glPopMatrix();
	} else {
		glBegin(GL_LINE_STRIP);
		glVertex3f(0, -0.5, 0);
		glVertex3f(alt, 0, 0);
		glVertex3f(0, 0.5, 0);
		glEnd();
	}
}

static void render_ship(struct ship *s, void *unused)
{
	Vec2 sp = S(s->physics->p);
	guint32 team_color = s->team->color;
	double x = sp.x, y = sp.y;
	double angle = s->gfx.angle;
	double scale = view_scale * s->class->radius;

	glPushMatrix();
	glTranslated(x, y, 0);
	glScaled(scale, scale, scale);
	glRotated(rad2deg(angle), 0, 0, 1);

	if (!strcmp(s->class->name, "mothership")) {
		int depth = MIN(MAX(log2(view_scale), 2), 8);
		glColor32(team_color | 0xEE);
		glPushMatrix();
		glScaled(0.5, 0.3, 0.3);
		render_circle(5);
		glPopMatrix();
		triangle_fractal(depth);
		glPushMatrix();
		glRotated(180, 0, 0, 1);
		triangle_fractal(depth);
		glPopMatrix();
	} else if (!strcmp(s->class->name, "fighter")) {
		glColor32(team_color | 0xAA);
		glBegin(GL_LINE_LOOP);
		glVertex3f(-0.70, -0.71, 0);
		glVertex3f(-0.70, 0.71, 0);
		glVertex3f(1, 0, 0);
		glEnd();
	} else if (!strcmp(s->class->name, "missile")) {
		glColor32(0x88888800 | 0x55);
		render_circle(5);
	} else if (!strcmp(s->class->name, "little_missile")) {
		glColor32(0x88888800 | 0x55);
		glBegin(GL_LINES);
		glVertex3f(-0.70, -0.71, 0);
		glVertex3f(-0.2, 0, 0);
		glVertex3f(-0.70, 0.71, 0);
		glVertex3f(-0.2, 0, 0);
		glVertex3f(-0.2, 0, 0);
		glVertex3f(1, 0, 0);
		glEnd();
	} else {
		glColor32(0x88888800 | 0x55);
		render_circle(8);
	}

	glPopMatrix();

	int tail_alpha_max = strstr(s->class->name, "missile") ? 16 : 64;
	glBegin(GL_LINE_STRIP);
	glColor32(team_color | tail_alpha_max);
	glVertex3f(x, y, 0);
	int i;
	for (i = 0; i < TAIL_SEGMENTS-1; i++) {
		int j = s->tail_head - i - 1;
		if (j < 0) j += TAIL_SEGMENTS;
		Vec2 sp2 = S(s->tail[j]);
		if (isnan(sp2.x))
			break;
		guint32 color = team_color | (tail_alpha_max-(tail_alpha_max/TAIL_SEGMENTS)*i);

		glColor32(color);
		glVertex3f(sp2.x, sp2.y, 0);
		sp = sp2;
	}
	glEnd();

	if (s == picked) {
		glColor32(0xCCCCCCAA);
		glPushMatrix();
		glTranslated(x, y, 0);
		glScaled(scale, scale, scale);
		render_circle(64);
		glPopMatrix();

		glColor32(0xCCCCCC77);
		glPushMatrix();
		glTranslated(x, y, 0);
		glScaled(view_scale, view_scale, view_scale);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(s->physics->thrust.x, s->physics->thrust.y, 0);
		glEnd();
		glPopMatrix();

		glColor32(0x49D5CEAA);
		glBegin(GL_LINE_STRIP);
		glVertex3f(x, y, 0);
		struct physics q = *s->physics;
		int i;
		double tick_length = 1/32.0;
		for (i = 0; i < 1/tick_length; i++) {
			physics_tick_one(&q, &tick_length);
			Vec2 sp = S(q.p);
			glVertex3f(sp.x, sp.y, 0);
		}
		glEnd();
	}

	if (s == picked || render_all_debug_lines) {
		glColor32(0x49D5CEAA);
		glBegin(GL_LINES);
		for (i = 0; i < s->debug.num_lines; i++) {
			Vec2 sa = S(s->debug.lines[i].a);
			Vec2 sb = S(s->debug.lines[i].b);
			glVertex3f(sa.x, sa.y, 0);
			glVertex3f(sb.x, sb.y, 0);
		}
		glEnd();
	}

	if (s == picked && s->dead) {
		picked = NULL;
	}
}

static void render_bullet(struct bullet *b, void *unused)
{
	if (b->dead) return;

	if (b->type == BULLET_SLUG) {
    Vec2 p1, p2, sp1, sp2;
		Vec2 dp = vec2_scale(b->physics->v, 1.0/64);
		Vec2 offset = vec2_scale(b->physics->v, g_rand_double(gfx_prng)/64);
		p1 = vec2_add(b->physics->p, offset);
    p2 = vec2_add(b->physics->p, vec2_add(offset, dp));
    sp1 = S(b->physics->p);
    sp2 = S(p2);

    glBegin(GL_LINE_STRIP);
    glColor32(0x44444455);
    glVertex3f(sp1.x, sp1.y, 0);
    glColor32(0x444444FF);
    glVertex3f(sp2.x, sp2.y, 0);
    glEnd();
	}
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

	g_list_foreach(all_ships, (GFunc)render_ship, NULL);
	g_list_foreach(all_bullets, (GFunc)render_bullet, NULL);
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

static void emit_bullet(struct bullet *b, void *unused)
{
	if (b->type == BULLET_PLASMA) {
		particle_shower(PARTICLE_PLASMA, b->physics->p, vec2(0,0), vec2_scale(b->physics->v, 1/63), MIN(b->physics->m/5,0.1), 3, 4, 6);
	}
}

static void emit_bullet_hit(struct bullet_hit *hit, void *unused)
{
	particle_shower(PARTICLE_HIT, hit->cp, vec2_scale(hit->s->physics->v, 1/32), vec2(0,0), 0.1, 1, 20, hit->e*100);
}

void emit_particles(void)
{
	g_list_foreach(all_ships, (GFunc)emit_ship, NULL);
	g_list_foreach(all_bullets, (GFunc)emit_bullet, NULL);
	g_list_foreach(bullet_hits, (GFunc)emit_bullet_hit, NULL);
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
