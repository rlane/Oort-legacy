namespace Oort {
	Game game;
	Renderer renderer;

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
	}

	public static void tick() {
		game.tick();
		renderer.tick();
		renderer.render();
		game.purge();
	}
}
