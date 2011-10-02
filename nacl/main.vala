namespace Oort {
	public static void start() {
		print("Oort starting\n");

		print("Loading ship classes\n");
		if (!ShipClass.load()) {
			print("Failed to load ship classes.\n");
		}

		print("Parsing scenario\n");
		var scn = Scenario.parse("scenarios/basic.json");
		string[] ai_filenames = { "examples/reference.lua", "examples/reference.lua" };
		print("Creating game\n");
		var game = new Game(0, scn, ai_filenames);
		print("Running game\n");

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

			game.tick();
			game.purge();

			unowned Team winner = game.check_victory();
			if (winner != null) {
				print("Team '%s' (%s) is victorious in %0.2f seconds\n", winner.name, winner.filename, game.ticks*Game.TICK_LENGTH);
				break;
			}

			sample_ticks++;
		}
		print("Done\n");
	}
}
