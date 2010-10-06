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
const double zoom_force = 0.1;

SDL_Surface *screen;
int screen_width = 640;
int screen_height = 480;
complex double view_pos = 0.0;
double view_scale = 16.0;
int paused = 0;
int single_step = 0;
int render_all_debug_lines = 0;
struct ship *picked = NULL;
int simple_graphics = 0;

static complex double W(complex double o)
{
	return view_pos + (o - (screen_width/2) - (I * screen_height/2))/view_scale;
}

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

static struct ship *pick(vec2 p)
{
	GList *es;
	for (es = g_list_first(all_ships); es; es = g_list_next(es)) {
		struct ship *s = es->data;
		if (distance(s->physics->p, p) < s->physics->r) {
			return s;
		}
	}
	return NULL;
}

static void zoom(double f)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	view_pos = (1-zoom_force)*view_pos + zoom_force * W(C(x,y));
	view_scale *= f;
}

int main(int argc, char **argv)
{
	SDL_Event event;

	font_init();
	memset(particles, 0, sizeof(particles));

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
			if (fps < 16 && !simple_graphics) {
				static int fps_fail;
				if (++fps_fail > 4) {
					printf("reverting to simple graphics\n");
					simple_graphics = 1;
				}
			}
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
				case SDLK_z:
					zoom(1.1);
					break;
				case SDLK_x:
					zoom(1.0/1.1);
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
					picked = pick(W(C(event.button.x, event.button.y)));
					break;
				case SDL_BUTTON_WHEELUP:
					zoom(1.1);
					break;
				case SDL_BUTTON_WHEELDOWN:
					zoom(1.0/1.1);
					break;
				default:
					break;
				}
			}
		}

		if (!paused) {
			game_tick(tick_length);
			if (!simple_graphics) {
				particle_tick();
			}
		}

		render_gl13();

		if (!paused) {
			game_purge();

			struct team *winner;
			if ((winner = game_check_victory())) {
				printf("Team '%s' is victorious in %0.2f seconds\n", winner->name, ticks*tick_length);
				return 0;
			}

			ticks += 1;
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
