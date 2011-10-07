namespace Oort {
	Game game;
	Renderer renderer;
	RenderPerf tick_perf;
	RenderPerf render_perf;

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
		renderer.reshape(800, 600);
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
