namespace Oort {
	Game game;
	Renderer renderer;
	RenderPerf tick_perf;
	RenderPerf render_perf;
	int width;
	int height;
	int mouse_x;
	int mouse_y;
	bool paused = false;
	bool single_step = false;
	bool need_redraw = true;

	public static void init() {
		message("Oort starting");
		Renderer.static_init();

		message("Loading ship classes");
		if (!ShipClass.load()) {
			message("Failed to load ship classes");
		}
	}

	public static void handle_message(string msg) {
		message("Received message: %s", msg);
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
		need_redraw = true;
	}

	public static bool handle_key(uint32 key) {
		if (key == 68) { // 'D'
			renderer.dump_perf();
		} else if (key == 88) { // 'X'
			renderer.zoom(mouse_x, mouse_y, 1.0/1.1);
		} else if (key == 90) { // 'Z'
			renderer.zoom(mouse_x, mouse_y, 1.1);
		} else if (key == ' ') {
			paused = !paused;
		} else if (key == 13) {
			paused = false;
			single_step = true;
		} else if (key == 86) {
			if (renderer.picked != null) {
				renderer.follow_picked = !renderer.follow_picked;
			}
		} else {
			message("unhandled key %u '%c'", key, (char)key);
			return false;
		}
		need_redraw = true;
		return true;
	}

	public static void handle_mouse_click(int x, int y, int button) {
		if (button == 0) {
			renderer.follow_picked = false;
			renderer.pick(x,y);
			need_redraw = true;
		}
	}

	public static void handle_mouse_move(int x, int y) {
		mouse_x = x;
		mouse_y = y;
	}

	public static void start() {
		message("Parsing scenario");
		var scn = Scenario.parse(Resources.load("scenarios/basic.json"));
		var ai = new AI() { filename="examples/reference.lua", code=Resources.load("examples/reference.lua") };
		message("Creating game");
		game = new Game(0, scn, { ai, ai });
		message("Initializing renderer");
		renderer = new Renderer(game, scn.initial_view_scale);
		renderer.init();
		renderer.reshape(width, height);
		message("Initialization complete");
		tick_perf = new RenderPerf();
		render_perf = new RenderPerf();
	}

	public static void tick() {
		if (!paused) {
			TimeVal tick_start_time = TimeVal();
			game.purge();
			game.tick();
			renderer.tick();
			tick_perf.update_from_time(tick_start_time);
			need_redraw = true;
		}

		if (single_step) {
			paused = true;
			single_step = false;
		}

		if (need_redraw) {
			TimeVal render_start_time = TimeVal();
			renderer.render();
			render_perf.update_from_time(render_start_time);

			renderer.render_text(10, 10, "tick: " + tick_perf.summary());
			renderer.render_text(10, 20, "render: " + render_perf.summary());

			need_redraw = false;
		}
	}
}
