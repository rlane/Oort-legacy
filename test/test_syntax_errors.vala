using Oort;

int main(string[] args) {
	Test.init(ref args);

	if (!Thread.supported ()) {
		error ("Cannot run without thread support.");
	}

	Log.set_always_fatal(0);

	Resources.init(args[0]);

	assert(ShipClass.load());

	Test.add_func ("/scenario/syntax_error", () => {
		try {
			Scenario.parse(Resources.load("test/scenarios/syntax_error.json"));
			assert(false);
		} catch (ScenarioParseError e) {
		} catch (Error e) {
			error("parse failed: %s", e.message);
		}
	});

	Test.add_func ("/ai/syntax_error", () => {
		try {
			var scn_single = Scenario.parse(Resources.load("test/scenarios/simple.json"));
			var ai = new AI() { filename="", code=Resources.load("test/ai/syntax_error.lua") };
			new Game(0, scn_single, {ai , ai });
		} catch (ScenarioLoadError e) {
		} catch (Error e) {
			error("init failed: %s", e.message);
		}
	});

	Test.run();

	return 0;
}
