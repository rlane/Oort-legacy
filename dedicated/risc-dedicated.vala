using RISC;

uint32 opt_seed;
int opt_max_ticks;

const OptionEntry[] options = {
	{ "seed", 's', 0, OptionArg.INT, &opt_seed, "Random number generator seed", null },
	{ "max-ticks", 0, 0, OptionArg.INT, &opt_max_ticks, "Exit after given number of ticks", null },
	{ null }
};

int main(string[] args) {
	GLib.Environment.set_application_name(Config.PACKAGE_NAME);

	Paths.init(args[0]);
	print("using data from %s\n", RISC.Paths.resource_dir.get_path());

	opt_seed = 0;
	opt_max_ticks = -1;

	var optctx = new OptionContext("");
	optctx.add_main_entries(options, null);
	try {
		optctx.parse(ref args);
	} catch (OptionError e) {
		print("%s\n", e.message);
		return 1;
	}

	if (!Thread.supported()) {
		print("Cannot run without thread support.\n");
		return 1;
	}

	if (!ShipClass.load(data_path("ships.lua"))) {
		print("Failed to load ship classes.\n");
		return 1;
	}

	var scenario_filename = args[1];
	var ai_filenames = args[2:(args.length)];

	var scn = Scenario.parse(scenario_filename);
	if (scn == null) {
		error("Failed to parse scenario");
	}

	Game game;
	try {
		game = new Game(opt_seed, scn, ai_filenames);
		if (!game.init()) {
			warning("Game initialization failed");
			return 1;
		}
	} catch (FileError e) {
		error("Game initialization failed: %s", e.message);
	}

	TimeVal last_sample_time = TimeVal();
	int sample_ticks = 0;
	bool callgrind_collection_started = false;

	while (true) {
		TimeVal now = TimeVal();
		long usecs = (now.tv_sec-last_sample_time.tv_sec)*(1000*1000) + (now.tv_usec - last_sample_time.tv_usec);
		if (usecs > 1000*1000) {
			print("%g FPS\n", (1000.0*1000*sample_ticks/usecs));
			sample_ticks = 0;
			last_sample_time = now;
		}

		if (opt_max_ticks >= 0 && game.ticks >= opt_max_ticks) {
			print("exiting after %d ticks\n", game.ticks);
			break;
		}

		if (game.ticks == 10) {
			callgrind_collection_started = true;
		}

		if (callgrind_collection_started) {
			Util.toggle_callgrind_collection();
		}

		game.tick();
		game.purge();

		if (callgrind_collection_started) {
			Util.toggle_callgrind_collection();
		}

		unowned Team winner = game.check_victory();
		if (winner != null) {
			print("Team '%s' (%s) is victorious in %0.2f seconds\n", winner.name, winner.filename, game.ticks*Game.TICK_LENGTH);
			break;
		}

		sample_ticks++;
	}

	return 0;
}
