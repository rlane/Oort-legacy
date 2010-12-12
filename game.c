#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <sys/time.h>
#include <glib.h>

#include "game.h"
#include "physics.h"
#include "ship.h"
#include "bullet.h"
#include "team.h"
#include "scenario.h"
#include "task.h"
#include "util.h"

GList *bullet_hits = NULL;
FILE *trace_file = NULL;

static void handle_bullet_hit(struct ship *s, struct bullet *b, Vec2 cp)
{
	b->dead = 1;
	if (b->team != s->team) {
		Vec2 dv = vec2_sub(s->physics->v, b->physics->v);
		double hit_energy = 0.5 * b->physics->m * vec2_abs(dv);
		s->hull -= hit_energy;
		if (s->hull <= 0) {
			s->dead = 1;
		}

		struct bullet_hit *hit = g_slice_new(struct bullet_hit);
		hit->s = s;
		hit->b = b;
		hit->cp = cp;
		hit->e = hit_energy;
		bullet_hits = g_list_prepend(bullet_hits, hit);
	}
}

static void check_bullet_hits(double tick_length)
{
	GList *es, *eb;
	for (es = g_list_first(all_ships); es; es = g_list_next(es)) {
		struct ship *s = es->data;
		for (eb = g_list_first(all_bullets); eb; eb = g_list_next(eb)) {
			struct bullet *b = eb->data;
			Vec2 cp;
			if (physics_check_collision(s->physics, b->physics, tick_length, &cp)) {
				handle_bullet_hit(s, b, cp);
			}
		}
	}
}

void game_tick(double tick_length)
{
	check_bullet_hits(tick_length);
}

static void free_bullet_hit(struct bullet_hit *h)
{
	g_slice_free(struct bullet_hit, h);
}

void game_purge(void)
{
	g_list_foreach(bullet_hits, (GFunc)free_bullet_hit, NULL);
	g_list_free(bullet_hits);
	bullet_hits = NULL;
}
