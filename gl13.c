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

#ifdef GDK_WINDOWING_QUARTZ
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

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

int screen_width = 640;
int screen_height = 480;
complex double view_pos = 0.0;
double view_scale = 16.0;
int paused = 0;
int single_step = 0;
int render_all_debug_lines = 0;
struct ship *picked = NULL;
int simple_graphics = 0;

static complex double S(complex double p)
{
	return (p - view_pos) * view_scale +
		     (screen_width/2) +
				 (I * screen_height/2);
}

static void glColor32(guint32 c)
{
	glColor4ub((c >> 24) & 0xFF, (c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
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

static void render_ship(struct ship *s, void *unused)
{
	complex double sp = S(s->physics->p);
	guint32 team_color = s->team->color;
	double x = creal(sp), y = cimag(sp);
	double angle = atan2(cimag(s->physics->v), creal(s->physics->v));
	double scale = view_scale * s->class->radius;

	glPushMatrix();
	glTranslated(x, y, 0);
	glScaled(scale, scale, scale);
	glRotated(rad2deg(angle), 0, 0, 1);

	if (!strcmp(s->class->name, "mothership")) {
		glColor32(team_color | 0xEE);
		render_circle(64);
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

	glBegin(GL_LINE_STRIP);
	glVertex3f(x, y, 0);
	int i;
	for (i = 0; i < TAIL_SEGMENTS-1; i++) {
		int j = s->tail_head - i - 1;
		if (j < 0) j += TAIL_SEGMENTS;
		complex double sp2 = S(s->tail[j]);
		if (isnan(sp2))
			break;
		guint32 color = team_color | (64-(64/TAIL_SEGMENTS)*i);

		glColor32(color);
		glVertex3f(creal(sp2), cimag(sp2), 0);
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

		glColor32(0x49D5CEAA);
		glBegin(GL_LINE_STRIP);
		glVertex3f(x, y, 0);
		struct physics q = *s->physics;
		int i;
		double tick_length = 32.0;
		for (i = 0; i < 1/tick_length; i++) {
			physics_tick_one(&q, &tick_length);
			vec2 sp = S(q.p);
			glVertex3f(creal(sp), cimag(sp), 0);
		}
		glEnd();
	}

	if (s == picked || render_all_debug_lines) {
		glColor32(0x49D5CEAA);
		glBegin(GL_LINES);
		for (i = 0; i < s->debug.num_lines; i++) {
			vec2 sa = S(s->debug.lines[i].a);
			vec2 sb = S(s->debug.lines[i].b);
			glVertex3f(creal(sa), cimag(sa), 0);
			glVertex3f(creal(sb), cimag(sb), 0);
		}
		glEnd();
	}

	if (s == picked && s->dead) {
		picked = NULL;
	}
}

static void render_bullet(struct bullet *b, void *unused)
{
	if (simple_graphics) {
		complex double p2, sp1, sp2;
		p2 = b->physics->p + b->physics->v/32;
		sp1 = S(b->physics->p);
		sp2 = S(p2);

		glBegin(GL_LINE_STRIP);
		glColor32(0xFF000000);
		glVertex3f(creal(sp1), cimag(sp1), 0);
		glColor32(0xFF0000FF);
		glVertex3f(creal(sp2), cimag(sp2), 0);
		glEnd();
	} else {
		if (!paused) {
			particle_shower(PARTICLE_BULLET, b->physics->p, b->physics->v/63, MIN(b->physics->m/5,0.1), 7, 8, 3);
		}
	}
}

static void render_bullet_hit(struct bullet_hit *hit, void *unused)
{
	if (simple_graphics) {
		complex double sp = S(hit->cp);
		double x = creal(sp), y = cimag(sp);
		glColor32(0xAAAA22FF);

		glBegin(GL_LINE_STRIP);
		glVertex3f(x-2, y-2, 0);
		glVertex3f(x+2, y+2, 0);
		glEnd();

		glBegin(GL_LINE_STRIP);
		glVertex3f(x+2, y-2, 0);
		glVertex3f(x-2, y+2, 0);
		glEnd();
	} else {
		if (!paused) {
			particle_shower(PARTICLE_HIT, hit->cp, 0.0f, 0.1f, 1, 20, hit->e*100);
		}
	}
}

static void render_particles(void)
{
	int i;
	for (i = 0; i < MAX_PARTICLES; i++) {
		struct particle *c = &particles[i];
		if (c->ticks_left == 0) continue;
		complex float p = S(c->p);
		if (c->type == PARTICLE_HIT) {
			glPointSize(3);
			glColor4ub(255, 200, 200, c->ticks_left*8);
		} else if (c->type == PARTICLE_BULLET) {
			glPointSize(1.2);
			glColor4ub(255, 0, 0, c->ticks_left*32);
		}
		glBegin(GL_POINTS);
		glVertex3f(creal(p), cimag(p), 0);
		glEnd();
	}
}

void render_gl13(void)
{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		g_list_foreach(all_ships, (GFunc)render_ship, NULL);
		g_list_foreach(all_bullets, (GFunc)render_bullet, NULL);
		g_list_foreach(bullet_hits, (GFunc)render_bullet_hit, NULL);

		if (!simple_graphics) {
			render_particles();
		}

		if (picked) {
			const int x = 15, y = 82, dy = 12;
			glColor32(0xAAFFFFAA);
			glPrintf(x, y-0*dy, "%s %.8s", picked->class->name, picked->api_id);
			glPrintf(x, y-1*dy, "hull: %.2f", picked->hull);
			glPrintf(x, y-2*dy, "position: " VEC2_FMT, VEC2_ARG(picked->physics->p));
			glPrintf(x, y-3*dy, "velocity: " VEC2_FMT, VEC2_ARG(picked->physics->v));
			glPrintf(x, y-4*dy, "thrust: " VEC2_FMT, VEC2_ARG(picked->physics->thrust));
			glPrintf(x, y-5*dy, "energy: %g", ship_get_energy(picked));
		}
}

static complex double W(complex double o)
{
	return view_pos + (o - (screen_width/2) - (I * screen_height/2))/view_scale;
}

struct ship *pick(int x, int y)
{
	vec2 p = W(C(x, y));
	GList *es;
	for (es = g_list_first(all_ships); es; es = g_list_next(es)) {
		struct ship *s = es->data;
		if (distance(s->physics->p, p) < s->physics->r) {
			return s;
		}
	}
	return NULL;
}

void zoom(int x, int y, double f)
{
	view_pos = (1-zoom_force)*view_pos + zoom_force * W(C(x,y));
	view_scale *= f;
}
