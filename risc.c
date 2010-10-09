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
#include "particle.h"
#include "glutil.h"
#include "renderer.h"

static FPSmanager fps_manager;

static const int FPS = 32;
static const double tick_length = 1.0/32.0;

SDL_Surface *screen;

static void get_resolution(int *width, int *height)
{
	char *s;

	const SDL_VideoInfo *vid_info = SDL_GetVideoInfo();
	*width = vid_info->current_w;
	*height = vid_info->current_h;

	if ((s = getenv("RISC_W"))) {
		*width = atoi(s);
	}

	if ((s = getenv("RISC_H"))) {
		*height = atoi(s);
	}

	printf("using resolution %dx%d\n", *width, *height);
}

int main(int argc, char **argv)
{
	SDL_Event event;
	int width, height;

	font_init();
	memset(particles, 0, sizeof(particles));

	printf("initializing SDL..\n");

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr,"Failed to initialize SDL Video!\n");
		exit(1);
	}

	get_resolution(&width, &height);

	SDL_initFramerate(&fps_manager);
	SDL_setFramerate(&fps_manager, FPS);

	SDL_WM_SetCaption("RISC", "RISC");
	SDL_ShowCursor(SDL_ENABLE);
	SDL_EnableKeyRepeat(200,100);

	atexit(SDL_Quit);

	printf("initializing OpenGL..\n");

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
	screen = SDL_SetVideoMode(width, height, 32, SDL_OPENGL | SDL_FULLSCREEN);

	if (!screen) {
		fprintf(stderr, "Failed to set video mode: %s\n", SDL_GetError());
		exit(1);
	}

	init_gl13();
	reshape_gl13(width, height);

	int seed = getpid() ^ time(NULL);

	int num_teams;
	char *scenario;
	char **teams;

	if (argc == 1) {
		scenario = NULL;
		num_teams = 0;
		teams = NULL;
	} else {
		scenario = argv[1];
		num_teams = argc - 2;
		teams = argv+2;
	}

	if (game_init(seed, scenario, num_teams, teams)) {
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
			double fps = (1000.0*1000*sample_ticks/usecs);
			printf("%g FPS\n", fps);
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

			int x,y;
			SDL_GetMouseState(&x, &y);

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_z:
					zoom(x, y, 1.1);
					break;
				case SDLK_x:
					zoom(x, y, 1.0/1.1);
					break;
				case SDLK_SPACE:
					paused = !paused;
					break;
				case SDLK_RETURN:
					paused = 0;
					single_step = 1;
					break;
				case SDLK_y:
					render_all_debug_lines = !render_all_debug_lines;
					break;
				case SDLK_p:
					screenshot("screenshot.tga");
					break;
				default:
					break;
				}
			}

			if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_WHEELUP ||
				    event.button.button == SDL_BUTTON_WHEELDOWN) {
				}

				switch (event.button.button) {
				case SDL_BUTTON_LEFT:
					pick(event.button.x, event.button.y);
					break;
				case SDL_BUTTON_WHEELUP:
					zoom(event.button.x, event.button.y, 1.1);
					break;
				case SDL_BUTTON_WHEELDOWN:
					zoom(event.button.x, event.button.y, 1.0/1.1);
					break;
				default:
					break;
				}
			}
		}

		if (!paused) {
			game_tick(tick_length);
			particle_tick();
		}

		render_gl13(paused);
		SDL_GL_SwapBuffers();

		if (!paused) {
			game_purge();

			struct team *winner;
			if ((winner = game_check_victory())) {
				printf("Team '%s' is victorious in %0.2f seconds\n", winner->name, ticks*tick_length);
				return 0;
			}
		}

		if (single_step) {
			paused = 1;
			single_step = 0;
		}

		SDL_framerateDelay(&fps_manager);
		sample_ticks++;
	}

	return 0;
}
