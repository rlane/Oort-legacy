using RISC;

double tick_length = 1.0/32;

int main(string[] args) {
	int seed = Util.envtol("RISC_SEED", 42);
	int max_ticks = Util.envtol("RISC_MAX_TICKS", -1);
	bool callgrind_collection_started = false;

	if (!Thread.supported ()) {
		error ("Cannot run without thread support.");
	}

	Paths.init(args[0]);
	print("using data from %s\n", RISC.Paths.resource_dir.get_path());

	int ret;
	try {
		if (args.length <= 1) {
			ret = Game.init(42, data_path("scenarios/demo1.lua"), { });
		} else {
			ret = Game.init(seed, args[1], args[2:(args.length)]);
		}
	} catch (FileError e) {
		error("Game initialization failed: %s", e.message);
	}

	if (ret != 0) {
		warning("Game initialization failed");
		return 1;
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

		if (max_ticks >= 0 && Game.ticks >= max_ticks) {
			print("exiting after %d ticks\n", Game.ticks);
			break;
		}

		if (Game.ticks == 10) {
			callgrind_collection_started = true;
		}

		if (callgrind_collection_started) {
			Util.toggle_callgrind_collection();
		}

		Game.tick(tick_length);
		Game.purge();

		if (callgrind_collection_started) {
			Util.toggle_callgrind_collection();
		}

		unowned Team winner = Game.check_victory();
		if (winner != null) {
			print("Team '%s' is victorious in %0.2f seconds\n", winner.name, Game.ticks*tick_length);
			break;
		}

		sample_ticks++;
	}

	Game.shutdown();

	return 0;
}
