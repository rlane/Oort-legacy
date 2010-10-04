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
#include <SDL_gfxPrimitives_font.h>
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

static const int FPS = 32;
static const double tick_length = 1.0/32.0;
const double zoom_force = 0.1;

static int screen_width = 640;
static int screen_height = 480;
static complex double view_pos = 0.0;
static double view_scale = 16.0;
static int paused = 0;
static int single_step = 0;
static int render_all_debug_lines = 0;
static struct ship *picked = NULL;
static GLubyte font[256*8];
static int simple_graphics = 0;

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

void physics_tick_one(struct physics *q, const double *ta);

static void render_ship(struct ship *s, void *unused)
{
	complex double sp = S(s->physics->p);
	Uint32 team_color = s->team->color;
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
		Uint32 color = team_color | (64-(64/TAIL_SEGMENTS)*i);

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
			particle_shower(PARTICLE_BULLET, b->physics->p, b->physics->v/63, 0.01f, 15, 16, 3);
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

static void glWrite(int x, int y, const char *str)
{
#ifndef WINDOWS
	glWindowPos2i(x, y);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

	char c;
	while ((c = *str++)) {
		glBitmap(8, 8, 4, 4, 9, 0, font + 8*c);
	}
#endif
}

static void glPrintf(int x, int y, const char *fmt, ...)
{
	static char buf[1024];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	glWrite(x, y, buf);
}

static void font_init(void)
{
	int i, j;
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 8; j++) {
			font[i*8 + j] = gfxPrimitivesFontdata[i*8 + (7-j)];
		}
	}
}

static void screenshot(void)
{
#ifndef WINDOWS
	char *filename = "screenshot.tga";
	int fd;
	struct tga_header *tga;
	size_t data_size = 3 * screen_width * screen_height;
	size_t map_size = (sizeof(*tga) + data_size + 4095) & (~0 << 12);
	
	if ((fd = open(filename, O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR)) < 0) {
		perror("open");
		return;
	}

	lseek(fd, sizeof(*tga) + data_size - 1, SEEK_SET);
	write(fd, "\x00", 1);

	if ((tga = mmap(NULL, map_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return;
	}

	*tga = tga_defaults;
	tga->width = screen_width;
	tga->height = screen_height;

	glReadPixels(0, 0, screen_width, screen_height, GL_BGR, GL_UNSIGNED_BYTE, tga->data);

	munmap(tga, map_size);
	close(fd);
#endif
}

int main(int argc, char **argv)
{
	SDL_Event event;

	font_init();

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
	glPointSize(2.5);

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
				printf("reverting to simple graphics\n");
				simple_graphics = 1;
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
					screenshot();
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

		SDL_GL_SwapBuffers();

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
