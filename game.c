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

int ticks = 0;
GList *bullet_hits = NULL;
double current_time = 0.0;
GRand *prng = NULL;
FILE *trace_file = NULL;

static const char *default_scenario = "scenarios/basic.lua";
static char *default_teams[] = { "examples/switch.lua", "examples/switch.lua" };

static void handle_bullet_hit(struct ship *s, struct bullet *b, vec2 cp)
{
	b->dead = 1;
	if (b->team != s->team) {
		complex double dv = s->physics->v - b->physics->v;
		double hit_energy = 0.5 * b->physics->m * distance(dv, 0);
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
			complex double cp;
			if (physics_check_collision(s->physics, b->physics, tick_length, &cp)) {
				handle_bullet_hit(s, b, cp);
			}
		}
	}
}

int game_init(int seed, const char *scenario, int num_teams, char **teams)
{
	task_init();

	prng = g_rand_new_with_seed(seed);

	printf("loading ships...\n");

	if (load_ship_classes("ships.lua")) {
		return 1;
	}

	printf("loading scenario...\n");

	if (scenario) {
		if (load_scenario(scenario, num_teams, teams)) {
			return 1;
		}
	} else {
		if (load_scenario(default_scenario, sizeof(default_teams)/sizeof(*default_teams), default_teams)) {
			return 1;
		}
	}

	return 0;
}

void game_tick(double tick_length)
{
	check_bullet_hits(tick_length);
	physics_tick(tick_length);
	ship_tick(tick_length);
	bullet_tick(tick_length);
	current_time += tick_length;
	ticks += 1;
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
	bullet_purge();
	ship_purge();
}

void game_shutdown(void)
{
	task_shutdown();
	ship_shutdown();
	bullet_shutdown();
	team_shutdown();
	g_rand_free(prng);
}

struct team *game_check_victory(void)
{
	struct team *winner = NULL;
	GList *e;

	for (e = g_list_first(all_ships); e; e = g_list_next(e)) {
		struct ship *s = e->data;
		if (!s->class->count_for_victory) continue;
		if (winner && s->team != winner) {
			return NULL;
		}
		winner = s->team;
	}

	return winner;
}
