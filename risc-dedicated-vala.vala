using RISC;
using RISC.C;

#if VALGRIND_VALA
[CCode (cheader_filename = "callgrind.h")]
bool callgrind_collection_started = 0;
#endif

double tick_length = 1.0/32;

int main(string[] args) {
	int seed = envtol("RISC_SEED", 42);
	int max_ticks = envtol("RISC_MAX_TICKS", -1);

	if (!Thread.supported ()) {
		error ("Cannot run without thread support.");
	}

	if (!RISC.find_data_dir()) {
		error("could not find data dir (set RISC_DATA)");
	}

	if (args.length <= 1) {
		game_init(42, data_path("scenarios/demo1.lua"), { });
	} else {
		game_init(seed, args[1], args[2:(args.length)]);
	}

	TimeVal last_sample_time = TimeVal();
	int sample_ticks = 0;

	while (true) {
		TimeVal now = TimeVal();
		long usecs = (now.tv_sec-last_sample_time.tv_sec)*(1000*1000) + (now.tv_usec - last_sample_time.tv_usec);
		if (usecs > 1000*1000) {
			print("%g FPS\n", (1000.0*1000*sample_ticks/usecs));
			sample_ticks = 0;
			last_sample_time = now;
		}

		if (max_ticks >= 0 && ticks >= max_ticks) {
			print("exiting after %d ticks\n", ticks);
			break;
		}

#if VALGRIND_VALA
		if (ticks == 10) {
			callgrind_collection_started = 1;
		}

		if (callgrind_collection_started) {
			CALLGRIND_TOGGLE_COLLECT;
		}
#endif

		game_tick(tick_length);
		game_purge();

#if VALGRIND_VALA
		if (callgrind_collection_started) {
			CALLGRIND_TOGGLE_COLLECT;
		}
#endif

		unowned Team winner = game_check_victory();
		if (winner != null) {
			print("Team '%s' is victorious in %0.2f seconds\n", winner.name, ticks*tick_length);
			break;
		}

		sample_ticks++;
	}

	game_shutdown();

	return 0;
}
