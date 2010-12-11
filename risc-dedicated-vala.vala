using RISC;

double tick_length = 1.0/32;

int main(string[] args) {
	if (!Thread.supported ()) {
		error ("Cannot run without thread support.");
	}

	if (!RISC.find_data_dir()) {
		error("could not find data dir (set RISC_DATA)");
	}

	if (args.length <= 1) {
		game_init(42, data_path("scenarios/demo1.lua"), { });
	} else {
		game_init(0, args[1], args[2:(args.length)]);
	}

	while (true) {
		game_tick(tick_length);

		game_purge();

		unowned Team winner = game_check_victory();

		if (winner != null) {
			print("Team '%s' is victorious in %0.2f seconds\n", winner.name, ticks*tick_length);
			break;
		}
	}

	game_shutdown();

	return 0;
}
