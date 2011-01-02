using RISC;

int main(string[] args) {
	Test.init(ref args);

	if (!Thread.supported ()) {
		error ("Cannot run without thread support.");
	}

	Log.set_always_fatal(0);

	Paths.init(args[0]);
	print("using data from %s\n", RISC.Paths.resource_dir.get_path());

	assert(ShipClass.load(data_path("ships.lua")));

	Test.add_func ("/scenario/syntax_error", () => {
		var scn = Scenario.parse(data_path("test/scenarios/syntax_error.json"));
		assert(scn == null);
	});

	Test.add_func ("/ai/syntax_error", () => {
		try {
			var scn_single = Scenario.parse(data_path("test/scenarios/simple.json"));
			var game = new Game(0, scn_single, { data_path("test/ai/syntax_error.lua"), data_path("test/ai/syntax_error.lua") });
			assert(!game.init());
		} catch (Error e) {
			error("init failed: %s", e.message);
		}
	});

	Test.add_func ("/ai/missing", () => {
		try {
			var scn_single = Scenario.parse(data_path("test/scenarios/simple.json"));
			var game = new Game(0, scn_single, { data_path("test/ai/missing.lua"), data_path("test/ai/missing.lua") });
			assert(!game.init());
		} catch (Error e) {
			error("init failed: %s", e.message);
		}
	});

	Test.run();

	return 0;
}
