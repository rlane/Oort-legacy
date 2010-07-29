#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <complex.h>
#include <sys/time.h>
#include <math.h>
#include <glib.h>

#include "game.h"
#include "physics.h"
#include "ship.h"
#include "bullet.h"
#include "scenario.h"
#include "team.h"

static const double tick_length = 1.0/32.0;

int main(int argc, char **argv)
{
	struct timeval last_sample_time;
	int sample_ticks = 0;
	int seed = getpid() ^ time(NULL);
	const char *scenario = argc > 1 ? argv[1] : "scenarios/basic.lua";

	if (game_init(seed, scenario)) {
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

		game_tick(tick_length);

		game_purge();

		struct team *winner;
		if ((winner = game_check_victory())) {
			printf("Team '%s' is victorious in %0.2f seconds\n", winner->name, ticks*tick_length);
			break;
		}

		ticks += 1;
		sample_ticks++;
	}

	game_shutdown();

	return 0;
}
