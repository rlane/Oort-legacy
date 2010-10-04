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
#include <SDL_framerate.h>
#include <SDL_opengl.h>

#include "game.h"
#include "physics.h"
#include "ship.h"
#include "bullet.h"
#include "scenario.h"
#include "team.h"
#include "tga.h"
#include "particle.h"

static SDL_Surface *screen;
static FPSmanager fps_manager;

static const int FPS = 60;
static const double tick_length = 1.0/32.0;
const double zoom_force = 0.1;
static complex double view_pos = 0.0;
static double view_scale = 64.0;

static int screen_width = 640;
static int screen_height = 480;

static void get_resolution(void)
{
	char *s;

	const SDL_VideoInfo *vid_info = SDL_GetVideoInfo();
	screen_width = vid_info->current_w;
	screen_height = vid_info->current_h;

	if ((s = getenv("RISC_W"))) {
		screen_width = atoi(s);
	}

	if ((s = getenv("RISC_H"))) {
		screen_height = atoi(s);
	}

	printf("using resolution %dx%d\n", screen_width, screen_height);
}

static complex double S(complex double p)
{
	return (p - view_pos) * view_scale +
		     (screen_width/2) +
				 (I * screen_height/2);
}

static void render_particles(void)
{
	int i;
	glBegin(GL_POINTS);
	for (i = 0; i < MAX_PARTICLES; i++) {
		struct particle *c = &particles[i];
		if (c->ticks_left == 0) continue;
		complex float p = S(c->p);
		if (c->type == PARTICLE_HIT) {
			glColor4ub(255, 200, 200, c->ticks_left*8);
		} else if (c->type == PARTICLE_BULLET) {
			glColor4ub(255, 0, 0, c->ticks_left*16);
		}
		glVertex3f(creal(p), cimag(p), 0);
	}
	glEnd();
}

int main(int argc, char **argv)
{
	SDL_Event event;

	printf("initializing SDL..\n");

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr,"Failed to initialize SDL Video!\n");
		exit(1);
	}

	get_resolution();

	SDL_initFramerate(&fps_manager);
	SDL_setFramerate(&fps_manager, FPS);

	SDL_WM_SetCaption("RISC", "RISC");
	SDL_ShowCursor(SDL_ENABLE);
	SDL_EnableKeyRepeat(200,100);

	atexit(SDL_Quit);

	printf("initializing OpenGL..\n");

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
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
	glPointSize(4.0);

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
		}

		//particle_shower(PARTICLE_BULLET, b->physics->p, b->physics->v/63, 0.01f, 8, 16, 6);

		complex double p = C(g_random_double_range(-10,10), g_random_double_range(-10,10));
		particle_shower(PARTICLE_HIT, p, 0.0f, 0.1f, 1, 60, 1000);


		particle_tick();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render_particles();
		SDL_GL_SwapBuffers();
		SDL_framerateDelay(&fps_manager);
	}

	return 0;
}
