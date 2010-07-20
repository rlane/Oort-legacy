#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <sys/time.h>
#include <math.h>
#include <glib.h>

#if 0
#include <GL/glu.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#endif

#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_framerate.h>
#include <SDL_opengl.h>

#include "game.h"
#include "physics.h"
#include "ship.h"
#include "bullet.h"

SDL_Surface *screen;
FPSmanager fps_manager;

const int screen_width = 1024;
const int screen_height = 768;
const double tick_length = 1.0/32.0;
complex double view_pos = 0.0;
double view_scale = 32.0;

struct team green_team = {
	.name = "green",
	.color = 0x00FF0000,
};

struct team blue_team = {
	.name = "blue",
	.color = 0x0000FF00,
};

static complex double S(complex double p)
{
	return (p - view_pos) * view_scale +
		     (screen_width/2) +
				 (I * screen_height/2);
}

static void glColor32(Uint32 c)
{
	glColor4ub((c >> 24) & 0xFF, (c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
}

static void render_octagon(double x, double y, double r)
{
	double da = 2*M_PI/8, a = 0;
	int i;

	glBegin(GL_LINE_STRIP);
	for (i = 0; i < 9; i++) {
		a += da;
		glVertex3f(x + cos(a)*r, y + sin(a)*r, 0);
	}
	glEnd();
}

static void render_ship(struct ship *s, void *unused)
{
	complex double sp = S(s->physics->p);
	double sr = s->class->r * view_scale;
	Uint32 team_color = s->team->color;
	double x = creal(sp), y = cimag(sp);

	glColor32(team_color | 0xAA);
	render_octagon(x, y, sr);

	glBegin(GL_LINE_STRIP);
	glVertex3f(x, y, 0);
	int i;
	for (i = 0; i < TAIL_SEGMENTS-1; i++) {
		int j = s->tail_head - i - 1;
		if (j < 0) j += TAIL_SEGMENTS;
		complex double sp2 = S(s->tail[j]);
		if (isnan(sp2))
			break;
		Uint32 color = team_color | (64-(64/TAIL_SEGMENTS)*i);

		glColor32(color);
		glVertex3f(creal(sp2), cimag(sp2), 0);
		aalineColor(screen, creal(sp), cimag(sp), creal(sp2), cimag(sp2), color);
		sp = sp2;
	}
	glEnd();
}

static void render_bullet(struct bullet *b, void *unused)
{
	complex double p2, sp1, sp2;
	p2 = b->physics->p + b->physics->v/32;
	sp1 = S(b->physics->p);
	sp2 = S(p2);

	glColor32(0xFF0000AA);
	glBegin(GL_LINE_STRIP);
	glVertex3f(creal(sp1), cimag(sp1), 0);
	glVertex3f(creal(sp2), cimag(sp2), 0);
	glEnd();
}

static void render_bullet_hit(struct bullet_hit *hit, void *unused)
{
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
}

int main(int argc, char **argv)
{
	SDL_Event event;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr,"Failed to initialize SDL Video!\n");
		exit(1);
	}

	atexit(SDL_Quit);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	screen = SDL_SetVideoMode(screen_width, screen_height, 32, SDL_OPENGL | SDL_FULLSCREEN);

	if (!screen)
	{
		fprintf(stderr,"Couldn't set video mode!\n%s\n", SDL_GetError());
		exit(1);
	}

	SDL_initFramerate(&fps_manager);
	SDL_setFramerate(&fps_manager, 32);

	SDL_WM_SetCaption("RISC", "RISC");
  SDL_ShowCursor(SDL_DISABLE);


	glEnable( GL_TEXTURE_2D );

	glClearColor( 0.0f, 0.0f, 0.03f, 0.0f );

	glViewport(0, 0, screen_width, screen_height);

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0f, screen_width, screen_height, 0.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POINT_SMOOTH);
	//glLineWidth(1.2);
	//glPointSize(1.0);

	printf("initialized opengl\n");

	Uint32 background_color = SDL_MapRGB(screen->format, 0, 0, 0);

	struct ship *s;

	s = ship_create("rock.lua", &mothership);
	s->physics->p = 2.0 + 2.0*I;
	s->team = &blue_team;

	const int n = 128;

	int i;
	for (i = 0; i < n; i++) {
		s = ship_create("orbit.lua", &fighter);
		s->physics->p = g_random_double_range(-9.0,-9.0) +
			              g_random_double_range(-2.0,2.0)*I;
		s->physics->v = g_random_double_range(0.0,0.1) +
			              g_random_double_range(1.0,1.3)*I;
		s->team = &green_team;
	}

	for (i = 0; i < n; i++) {
		s = ship_create("orbit.lua", &fighter);
		s->physics->p = g_random_double_range(8.0,9.0) +
			              g_random_double_range(-2.0,2.0)*I;
		s->physics->v = g_random_double_range(0.0,0.1) +
			              g_random_double_range(-1.3,-1.0)*I;
		s->team = &blue_team;
	}

	struct timeval last_sample_time;
	int sample_ticks = 0;

	gettimeofday(&last_sample_time, NULL);

	while (1) {
		if (sample_ticks == 32*1) {
			struct timeval now;
			gettimeofday(&now, NULL);
			long usecs = (now.tv_sec-last_sample_time.tv_sec)*(1000*1000) + (now.tv_usec - last_sample_time.tv_usec);
			printf("%g FPS\n", (1000.0*1000*sample_ticks/usecs));
			sample_ticks = 0;
			last_sample_time = now;
		}

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT ||
					(event.type == SDL_KEYDOWN &&
						event.key.keysym.sym == SDLK_ESCAPE)) {
				SDL_Quit();
				return 0;
			}

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_UP:
					view_pos -= C(0,1);
					break;
				case SDLK_DOWN:
					view_pos += C(0,1);
					break;
				case SDLK_LEFT:
					view_pos -= C(1,0);
					break;
				case SDLK_RIGHT:
					view_pos -= C(-1,0);
					break;
				case SDLK_z:
					view_scale *= 1.1;
					break;
				case SDLK_x:
					view_scale /= 1.1;
					break;
				default:
					break;
				}
			}
		}

		game_tick(tick_length);

#if 0
		complex double sun_p = S(0);
		aacircleColor(screen, creal(sun_p), cimag(sun_p), 20, 0xAAAA22FF);

		complex double v1 = S(border_v1);
		complex double v2 = S(border_v2);
		rectangleRGBA(screen,
				          creal(v1), cimag(v1), creal(v2), cimag(v2),
				          255, 0, 0, 255);
#endif

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		g_list_foreach(all_ships, (GFunc)render_ship, NULL);
		g_list_foreach(all_bullets, (GFunc)render_bullet, NULL);
		g_list_foreach(bullet_hits, (GFunc)render_bullet_hit, NULL);

		SDL_GL_SwapBuffers();

		game_purge();

		SDL_framerateDelay(&fps_manager);
		ticks += 1;
		sample_ticks++;
	}

	return 0;
}
