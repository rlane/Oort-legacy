/*
 * Reactor/Batteries per ship.
 * Basic ships can thrust/fire in any direction.
 * Thrust/fire/spawn/shield consume energy.
 */

#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <sys/time.h>

#include <glib.h>

#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_framerate.h>

#include "risc.h"
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
const complex double border_v1 = -16.0 + -12.0*I;
const complex double border_v2 = 15.95 + 11.95*I;
int ticks = 0;

struct team green_team = {
	.name = "green",
	.color = 0x00FF0000,
};

struct team blue_team = {
	.name = "blue",
	.color = 0x0000FF00,
};

complex double S(complex double p)
{
	return (p - view_pos) * view_scale +
		     (screen_width/2) +
				 (I * screen_height/2);
}

static void render_ship(struct ship *s, void *unused)
{
	complex double sp = S(s->physics->p);
	double sr = s->class->r * view_scale;
	Uint32 team_color = s->team->color;
	aacircleColor(screen, creal(sp), cimag(sp), sr, team_color | 150);

	int i;
	for (i = 0; i < TAIL_SEGMENTS-1; i++) {
		int j = s->tail_head - i - 1;
		if (j < 0) j += TAIL_SEGMENTS;
		complex double sp2 = S(s->tail[j]);
		if (isnan(sp2))
			break;
		int sp2_clipped = creal(sp2) < 0 || creal(sp2) > screen_width ||
			                cimag(sp2) < 0 || cimag(sp2) > screen_height;
		Uint32 color = team_color | (64-(64/TAIL_SEGMENTS)*i);

		Uint32 old_color;
		if (!sp2_clipped) {
			old_color = ((Uint32*)screen->pixels)[(int)creal(sp2) + (int)cimag(sp2)*screen_width];
		}
		aalineColor(screen, creal(sp), cimag(sp), creal(sp2), cimag(sp2), color);
		if (!sp2_clipped) {
			pixelColor(screen, creal(sp2), cimag(sp2), old_color | 0xFF);
		}
		sp = sp2;
	}
}

static void render_bullet(struct bullet *b, void *unused)
{
	complex double p2, sp1, sp2;
	p2 = b->physics->p + b->physics->v/32;
	sp1 = S(b->physics->p);
	sp2 = S(p2);
	aalineColor(screen, creal(sp1), cimag(sp1), creal(sp2), cimag(sp2), 0xFF0000AA);
}

static void handle_bullet_hit(struct ship *s, struct bullet *b, vec2 cp)
{
	b->dead = 1;
	if (b->team != s->team) {
		complex double dv = s->physics->v - b->physics->v;
		double hit_energy = 0.5 * b->physics->m * distance(dv, 0);
		complex double exp_p = S(cp);
		aacircleColor(screen, creal(exp_p), cimag(exp_p), 5, 0xAAAA22FF);
		s->hull -= hit_energy;
		if (s->hull <= 0) {
			s->dead = 1;
		}
	}
}

int main(int argc, char **argv)
{
	SDL_Event event;

	if (SDL_Init( SDL_INIT_VIDEO ) != 0) {
    fprintf( stderr, "Could not initialise SDL: %s\n", SDL_GetError() );
    return 1;
  }

	if ((screen = SDL_SetVideoMode(screen_width, screen_height, 32, SDL_SWSURFACE|SDL_FULLSCREEN)) == NULL) {
    fprintf( stderr, "Could not set SDL video mode: %s\n", SDL_GetError() );
    return 1;
  }

	SDL_initFramerate(&fps_manager);
	SDL_setFramerate(&fps_manager, 32);

	SDL_WM_SetCaption("RISC", "RISC");
  SDL_ShowCursor(SDL_DISABLE);

	Uint32 background_color = SDL_MapRGB(screen->format, 0, 0, 0);

	struct ship *s;

	s = ship_create("rock.lua", &mothership);
	s->physics->p = 2.0 + 2.0*I;
	s->team = &blue_team;

	int i;
	for (i = 0; i < 16; i++) {
		s = ship_create("orbit.lua", &fighter);
		s->physics->p = g_random_double_range(-9.0,-9.0) +
			              g_random_double_range(-2.0,2.0)*I;
		s->physics->v = g_random_double_range(0.0,0.1) +
			              g_random_double_range(1.0,1.3)*I;
		s->team = &green_team;
	}

	for (i = 0; i < 16; i++) {
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

		physics_tick(tick_length);
		ship_tick(tick_length);
		bullet_tick(tick_length);

		SDL_FillRect(screen, NULL, background_color);

		SDL_LockSurface(screen);

		complex double sun_p = S(0);
		aacircleColor(screen, creal(sun_p), cimag(sun_p), 20, 0xAAAA22FF);

		complex double v1 = S(border_v1);
		complex double v2 = S(border_v2);
		rectangleRGBA(screen,
				          creal(v1), cimag(v1), creal(v2), cimag(v2),
				          255, 0, 0, 255);

		g_list_foreach(all_ships, (GFunc)render_ship, NULL);
		g_list_foreach(all_bullets, (GFunc)render_bullet, NULL);

		GList *es, *eb;
		for (es = g_list_first(all_ships); es; es = g_list_next(es)) {
			struct ship *s = es->data;
			for (eb = g_list_first(all_bullets); eb; eb = g_list_next(eb)) {
				struct bullet *b = eb->data;
				complex double cp;
				if (physics_check_collision(s->physics, b->physics, tick_length, &cp)) {
					handle_bullet_hit(s, b, cp);
				}
			}
		}

		SDL_UnlockSurface(screen);

		SDL_Flip(screen);

		bullet_purge();
		ship_purge();

		SDL_framerateDelay(&fps_manager);
		ticks += 1;
		sample_ticks++;
	}

	SDL_Quit();
	return 0;
}
