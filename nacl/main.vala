namespace Oort {
	Game game;
	Renderer renderer;
	RenderPerf tick_perf;
	RenderPerf render_perf;
	int width;
	int height;

	public static void init() {
		print("Oort starting\n");
		Renderer.static_init();

		print("Loading ship classes\n");
		if (!ShipClass.load()) {
			print("Failed to load ship classes.\n");
		}
	}

	public static void handle_message(string msg) {
		print("Received message: %s\n", msg);
		string[] args;
		Shell.parse_argv(msg, out args);
		if (args[0] == "start") {
			start();
		}
	}

	public static void reshape(int w, int h) {
		message("reshape: w=%d h=%d", w, h);
		width = w;
		height = h;
		if (renderer != null) {
			renderer.reshape(width, height);
		}
	}

	public static bool handle_key(uint32 key) {
		if (key == 68) {
			renderer.dump_perf();
			return true;
		} else {
			message("unhandled key %u '%c'", key, (char)key);
			return false;
		}
	}

	public static void start() {
		print("Parsing scenario\n");
		var scn = Scenario.parse(Resources.load("scenarios/basic.json"));
		var ai = new AI() { filename="examples/reference.lua", code=Resources.load("examples/reference.lua") };
		print("Creating game\n");
		game = new Game(0, scn, { ai, ai });
		print("Initializing renderer\n");
		renderer = new Renderer(game, scn.initial_view_scale);
		renderer.init();
		renderer.reshape(width, height);
		print("Initialization complete");
		tick_perf = new RenderPerf();
		render_perf = new RenderPerf();
	}

	public static void tick() {
		TimeVal tick_start_time = TimeVal();
		game.purge();
		game.tick();
		renderer.tick();
		tick_perf.update_from_time(tick_start_time);

		TimeVal render_start_time = TimeVal();
		renderer.render();
		render_perf.update_from_time(render_start_time);

		renderer.render_text(10, 10, "tick: " + tick_perf.summary());
		renderer.render_text(10, 20, "render: " + render_perf.summary());
	}
}
