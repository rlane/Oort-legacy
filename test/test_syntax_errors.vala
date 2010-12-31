using RISC;

int main(string[] args) {
	Test.init(ref args);

	if (!Thread.supported ()) {
		error ("Cannot run without thread support.");
	}

	Log.set_always_fatal(0);

	Paths.init(args[0]);
	print("using data from %s\n", RISC.Paths.resource_dir.get_path());

	Test.add_func ("/scenario/syntax_error", () => {
		var ret = Game.init(0, data_path("test/scenarios/syntax_error.lua"), { });
		print("%d", ret);
		assert(ret == 1);
		Game.shutdown();
	});

	Test.add_func ("/ai/syntax_error", () => {
		var ret = Game.init(0, data_path("test/scenarios/single.lua"), { data_path("test/ai/syntax_error.lua") });
		print("%d", ret);
		assert(ret == 1);
		Game.shutdown();
	});

	Test.run();

	return 0;
}
