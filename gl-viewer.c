#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <complex.h>
#include <sys/time.h>
#include <math.h>
#include <glib.h>

#include <SDL.h>
#include <SDL_framerate.h>
#include <SDL_opengl.h>

#include "game.h"
#include "physics.h"
#include "ship.h"
#include "bullet.h"
#include "scenario.h"
#include "team.h"

static SDL_Surface *screen;
static FPSmanager fps_manager;

static const int FPS = 32;
static const double tick_length = 1.0/32.0;
const double zoom_force = 0.1;

static int screen_width = 1024;
static int screen_height = 768;
static complex double view_pos = 0.0;
static double view_scale = 8.0;

static complex double S(complex double p)
{
	return (p - view_pos) * view_scale +
		     (screen_width/2) +
				 (I * screen_height/2);
}

static complex double W(complex double o)
{
	return view_pos + (o - (screen_width/2) - (I * screen_height/2))/view_scale;
}

static void glColor32(Uint32 c)
{
	glColor4ub((c >> 24) & 0xFF, (c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
}

static void render_circle(double x, double y, double r)
{
	int n = r < 8 ? 8 : r+1;
	double da = 2*M_PI/n, a = 0;
	int i;

	glBegin(GL_LINE_STRIP);
	for (i = 0; i < (n+1); i++) {
		a += da;
		glVertex3f(x + cos(a)*r, y + sin(a)*r, 0);
	}
	glEnd();
}

static void render_ship(struct ship *s, void *unused)
{
	complex double sp = S(s->physics->p);
	double sr = s->class->radius * view_scale;
	Uint32 team_color = s->team->color;
	double x = creal(sp), y = cimag(sp);

	if (!strcmp(s->class->name, "mothership")) {
		glColor32(team_color | 0xEE);
	} else if (!strcmp(s->class->name, "fighter")) {
		glColor32(team_color | 0xAA);
	} else {
		glColor32(0x88888800 | 0x55);
	}

	render_circle(x, y, sr);

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

	glBegin(GL_LINE_STRIP);
	glColor32(0xFF000000);
	glVertex3f(creal(sp1), cimag(sp1), 0);
	glColor32(0xFF0000FF);
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

static void get_resolution(void)
{
	char *s;

	if ((s = getenv("RISC_W"))) {
		screen_width = atoi(s);
	}

	if ((s = getenv("RISC_H"))) {
		screen_height = atoi(s);
	}
}

int main(int argc, char **argv)
{
	SDL_Event event;

	get_resolution();

	printf("initializing SDL..\n");

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr,"Failed to initialize SDL Video!\n");
		exit(1);
	}

	SDL_initFramerate(&fps_manager);
	SDL_setFramerate(&fps_manager, FPS);

	SDL_WM_SetCaption("RISC", "RISC");
	SDL_ShowCursor(SDL_ENABLE);

	atexit(SDL_Quit);

	printf("initializing OpenGL..\n");

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	screen = SDL_SetVideoMode(screen_width, screen_height, 32, SDL_OPENGL | SDL_FULLSCREEN);

	if (!screen) {
		fprintf(stderr, "Failed to set video mode: %s\n", SDL_GetError());
		exit(1);
	}

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
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glLineWidth(1.2);
	glPointSize(1.0);

	int seed = getpid() ^ time(NULL);
	const char *scenario = argc > 1 ? argv[1] : "scenarios/basic.lua";

	if (game_init(seed, scenario)) {
		fprintf(stderr, "initialization failed\n");
		return 1;
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

			if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_WHEELUP ||
				    event.button.button == SDL_BUTTON_WHEELDOWN) {
					int x, y;
					SDL_GetMouseState(&x, &y);
					view_pos = (1-zoom_force)*view_pos + zoom_force * W(C(x,y));
				}

				switch (event.button.button) {
				case SDL_BUTTON_WHEELUP:
					view_scale *= 1.1;
					break;
				case SDL_BUTTON_WHEELDOWN:
					view_scale /= 1.1;
					break;
				default:
					break;
				}
			}
		}

		game_tick(tick_length);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		g_list_foreach(all_ships, (GFunc)render_ship, NULL);
		g_list_foreach(all_bullets, (GFunc)render_bullet, NULL);
		g_list_foreach(bullet_hits, (GFunc)render_bullet_hit, NULL);

		SDL_GL_SwapBuffers();

		game_purge();

		struct team *winner;
		if ((winner = game_check_victory())) {
			printf("Team '%s' is victorious in %0.2g seconds\n", winner->name, ticks*tick_length);
			return 0;
		}

		SDL_framerateDelay(&fps_manager);
		ticks += 1;
		sample_ticks++;
	}

	return 0;
}
