#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <complex.h>
#include <sys/time.h>
#include <math.h>
#include <glib.h>

#define VALGRIND
#ifdef VALGRIND
#include <valgrind/callgrind.h>
#endif

#include "game.h"
#include "physics.h"
#include "ship.h"
#include "bullet.h"
#include "scenario.h"
#include "team.h"
#include "util.h"

static const double tick_length = 1.0/32.0;

int main(int argc, char **argv)
{
	struct timeval last_sample_time;
	int sample_ticks = 0;
	int seed = envtol("RISC_SEED", getpid() ^ time(NULL));
	int max_ticks = envtol("RISC_MAX_TICKS", -1);
	char *trace_filename;

	if ((trace_filename = getenv("RISC_TRACE"))) {
		trace_file = fopen(trace_filename, "w");
		if (!trace_file) {
			fprintf(stderr, "could not open trace file\n");
			return 1;
		}
	}

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

	if (game_init(seed, scenario, teams, num_teams)) {
		fprintf(stderr, "initialization failed\n");
		return 1;
	}

	gettimeofday(&last_sample_time, NULL);

	while (1) {
		struct timeval now;
		gettimeofday(&now, NULL);
		long usecs = (now.tv_sec-last_sample_time.tv_sec)*(1000*1000) + (now.tv_usec - last_sample_time.tv_usec);
		if (usecs > 1000*1000) {
			printf("%g FPS\n", (1000.0*1000*sample_ticks/usecs));
			sample_ticks = 0;
			last_sample_time = now;
		}

		if (max_ticks >= 0 && ticks >= max_ticks) {
			printf("exiting after %d ticks\n", ticks);
			break;
		}

#ifdef VALGRIND
		if (ticks > 10) {
			CALLGRIND_START_INSTRUMENTATION;
		}
#endif

		game_tick(tick_length);

#ifdef VALGRIND
		if (ticks > 10) {
			CALLGRIND_STOP_INSTRUMENTATION;
		}
#endif

		game_purge();

		struct team *winner;
		if ((winner = game_check_victory())) {
			printf("Team '%s' is victorious in %0.2f seconds\n", winner->name, ticks*tick_length);
			break;
		}

		sample_ticks++;
	}

	game_shutdown();

	return 0;
}
