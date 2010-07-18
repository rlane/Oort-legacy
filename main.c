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

complex double S(complex double p)
{
	return (p - view_pos) * view_scale +
		     (screen_width/2) +
				 (I * screen_height/2);
}

static void render_ship(struct ship *s, void *unused)
{
	complex double sp = S(s->physics->p);
	double sr = s->class->r / view_scale;
	aacircleRGBA(screen, creal(sp), cimag(sp), sr, 0, 255, 0, 150);

	int i;
	for (i = 0; i < TAIL_SEGMENTS-1; i++) {
		int j = s->tail_head - i - 1;
		if (j < 0) j += TAIL_SEGMENTS;
		complex double sp2 = S(s->tail[j]);
		if (isnan(sp2))
			break;
		int sp2_clipped = creal(sp2) < 0 || creal(sp2) > screen_width ||
			                cimag(sp2) < 0 || cimag(sp2) > screen_height;
		Uint32 color = 0x00FF0000 + 64-(64/TAIL_SEGMENTS)*i;

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
	complex double sp = S(b->physics->p);
	pixelColor(screen, creal(sp), cimag(sp), 0xFF0000AA);
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

	int i;
	for (i = 0; i < 16; i++) {
		s = ship_create("orbit.lua");
		s->physics->p = g_random_double_range(-1.0,1.0) +
			              g_random_double_range(-1.0,1.0)*I;
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

		SDL_UnlockSurface(screen);

		SDL_Flip(screen);

		SDL_framerateDelay(&fps_manager);
		ticks += 1;
		sample_ticks++;
	}

	SDL_Quit();
	return 0;
}
