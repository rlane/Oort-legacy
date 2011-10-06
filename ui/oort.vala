using Gtk;
using Gdk;
using Lua;
using Oort;
using GLEW;

uint32 opt_seed;

const OptionEntry[] options = {
	{ "seed", 's', 0, OptionArg.INT, ref opt_seed, "Random number generator seed", null },
	{ null }
};

namespace Oort {
	class MenuBuilder : GLib.Object {
		public delegate void MenuAction();
		public void leaf(MenuShell parent, string label, MenuAction action) {
			var item = new MenuItem.with_mnemonic(label);
			parent.append(item);
			item.activate.connect((widget) => action());
		}

		public delegate void MenuBuilder(MenuShell parent);
		public void menu(MenuShell parent, string label, MenuBuilder builder) {
			var item = new MenuItem.with_mnemonic(label);
			var menu = new Menu();
			item.set_submenu(menu);
			parent.append(item);
			builder(menu);
		}
	}

	class MainWindow : Gtk.Window {
		public bool paused;

		private DrawingArea drawing_area;
		private bool single_step = false;
		private bool show_fps = false;
		private unowned Team winner = null;
		private Renderer renderer;
		private Mutex tick_lock;
		private unowned Thread<void*> ticker;
		private bool shutting_down = false;
		private Game game;

		private long frame_usecs = 0;
		private long sample_usecs = 0;

		enum GameState {
			DEMO,
			RUNNING,
			FINISHED,
		}

		private GameState game_state;

		public signal void ticked();

		public MainWindow() throws Error {
			this.title = "Oort";
			this.destroy.connect(shutdown);
			set_reallocate_redraws(true);

			this.tick_lock = new Mutex();

			var vbox = new VBox(false, 0);
			vbox.pack_start(make_menubar(), false, false, 0);
			vbox.pack_start(make_drawing_area(), true, true, 0);
			add(vbox);
			show_all();
		}

		private MenuBar make_menubar() {
			var menubar = new MenuBar();
			var b = new MenuBuilder();

			b.menu(menubar, "_Game", parent => {
				b.leaf(parent, "_New", new_game);
				b.leaf(parent, "_Stop", start_demo_game);
				b.leaf(parent, "_Pause", toggle_paused);
				b.leaf(parent, "_Single step", do_single_step);
				b.leaf(parent, "_Quit", shutdown);
			});

			b.menu(menubar, "_View", parent => {
				b.leaf(parent, "Zoom _in", menu_zoom_in);
				b.leaf(parent, "Zoom _out", menu_zoom_out);
				b.leaf(parent, "_Screenshot", show_screenshot_dialog);
				b.leaf(parent, "All _debug lines", toggle_render_all_debug_lines);
				b.leaf(parent, "_Framerate", toggle_show_fps);
				b.leaf(parent, "Follo_w ship", toggle_follow_picked);
				b.leaf(parent, "_Control ship", toggle_control_picked);
			});

			b.menu(menubar, "_Help", parent => {
				b.leaf(parent, "_About", show_about);
			});

			return menubar;
		}

		private DrawingArea make_drawing_area() {
			drawing_area = new DrawingArea();
			drawing_area.set_size_request(1024, 768);

			var glconfig = new GLConfig.by_mode(GLConfigMode.RGBA | GLConfigMode.DOUBLE);
			WidgetGL.set_gl_capability(drawing_area, glconfig, null, true, GLRenderType.RGBA_TYPE);

			drawing_area.add_events(Gdk.EventMask.BUTTON_PRESS_MASK);
			drawing_area.realize.connect(on_realize_event);
			drawing_area.configure_event.connect(on_configure_event);
			drawing_area.expose_event.connect(on_expose_event);
			key_press_event.connect(on_key_press_event);
			key_release_event.connect(on_key_release_event);
			drawing_area.button_press_event.connect(on_button_press_event);
			drawing_area.scroll_event.connect(on_scroll_event);

			return drawing_area;
		}

		public void new_game() {
			var scenario_chooser = new FileChooserDialog("Select scenario", this, Gtk.FileChooserAction.OPEN,
			                                             Gtk.Stock.OK, Gtk.ResponseType.ACCEPT,
			                                             Gtk.Stock.CANCEL, Gtk.ResponseType.REJECT);
			scenario_chooser.set_current_folder(Resources.path("scenarios"));
			try {
				scenario_chooser.add_shortcut_folder(Resources.path("scenarios"));
			} catch (GLib.Error e) {}
			scenario_chooser.response.connect( (response_id) => {
				if (response_id == Gtk.ResponseType.ACCEPT) {
					configure_scenario(scenario_chooser.get_filename());
				}
				scenario_chooser.destroy();
			});
			scenario_chooser.show();
		}

		public void show_screenshot_dialog() {
			var saved_paused = paused;
			paused = true;
			var chooser = new FileChooserDialog("Save screenshot", this, Gtk.FileChooserAction.SAVE,
			                                    Gtk.Stock.OK, Gtk.ResponseType.ACCEPT,
			                                    Gtk.Stock.CANCEL, Gtk.ResponseType.REJECT);
			chooser.set_current_name("screenshot.tga");
			chooser.response.connect( (response_id) => {
				if (response_id == Gtk.ResponseType.ACCEPT) {
					screenshot(chooser.get_filename());
				}
				chooser.destroy();
				paused = saved_paused;
			});
			chooser.show();
		}

		public void show_about() {
			var w = new AboutDialog();
			w.transient_for = this;
			w.authors = { "Rich Lane", null };
			w.version = Config.VERSION;
			w.response.connect( (response_id) => { w.destroy(); });
			w.show();
		}

		const int million = 1000*1000;
		private void *run() {
			long usecs_target = (long) (million*Game.TICK_LENGTH);
			TimeVal last = TimeVal();
			TimeVal sample = last;
			while (true) {
				if (shutting_down) break;
				tick_lock.lock();
				tick();
				tick_lock.unlock();
				TimeVal now = TimeVal();
				frame_usecs = (7*frame_usecs + (now.tv_sec-last.tv_sec)*million + (now.tv_usec - last.tv_usec))/8;
				sample_usecs = (7*sample_usecs + (now.tv_sec-sample.tv_sec)*million + (now.tv_usec - sample.tv_usec))/8;
				sample = TimeVal();
				long usecs = (now.tv_sec-last.tv_sec)*million + (now.tv_usec - last.tv_usec);
				Thread.usleep(long.max(usecs_target - usecs, 1000));
				last = TimeVal();
			}
			return null;
		}

		private bool tick() {
			if (game != null && !paused) {
				game.purge();
				game.tick();

				if (renderer != null) {
					renderer.tick();
				}

				if (game_state == GameState.RUNNING) {
					winner = game.check_victory();
					if (winner != null) {
						game_state = GameState.FINISHED;
					}
				}
			}

			if (single_step) {
				paused = true;
				single_step = false;
			}

			Timeout.add(0, after_tick);

			return true;
		}

		private bool after_tick() {
			var window = drawing_area.window;
			window.invalidate_rect((Rectangle)drawing_area.allocation, false);
			ticked();
			return false;
		}

		/* Widget is resized */
		private bool on_configure_event(Widget widget, EventConfigure event) {
			GLContext glcontext = WidgetGL.get_gl_context(widget);
			GLDrawable gldrawable = WidgetGL.get_gl_drawable(widget);

			if (!gldrawable.gl_begin(glcontext))
				return false;

			if (renderer != null) {
				renderer.reshape(widget.allocation.width, widget.allocation.height);
			}

			gldrawable.gl_end();
			return true;
		}

		/* Widget is asked to paint itself */
		private bool on_expose_event(Widget widget, EventExpose event) {
			GLContext glcontext = WidgetGL.get_gl_context(widget);
			GLDrawable gldrawable = WidgetGL.get_gl_drawable(widget);

			var rect = drawing_area.allocation;

			if (renderer == null) return true;

			if (!gldrawable.gl_begin(glcontext))
				return false;

			if (!tick_lock.trylock()) return true;

			renderer.render();
			
			if (show_fps && frame_usecs != 0 && sample_usecs != 0) {
				renderer.textf(rect.width-9*9, 15, "FPS: %.1f", (1000*1000.0)/sample_usecs);
				renderer.textf(rect.width-14*9, 25, "ms/frame: %.1f", renderer.perf.last_frame_time);
			}

			switch (game_state) {
			case GameState.DEMO:
				renderer.textf(rect.width/2-12*9, 50, "Click Game/New to begin");
				break;
			case GameState.RUNNING:
				break;
			case GameState.FINISHED:
				renderer.textf(rect.width/2-4*20, 50, "%s is victorious", winner.name);
				break;
			}

			gldrawable.swap_buffers();

			gldrawable.gl_end();

			tick_lock.unlock();
			return true;
		}

		private void on_realize_event(Widget widget) {
			GLContext glcontext = WidgetGL.get_gl_context(widget);
			GLDrawable gldrawable = WidgetGL.get_gl_drawable(widget);

			if (!gldrawable.gl_begin(glcontext))
				return;

			if (GLEW.init()) {
				error("GLEW initialization failed");
			}

			Renderer.static_init();

			gldrawable.gl_end();
		}

		private bool on_key_press_event(Widget widget, EventKey event) {
			int x, y;
			get_pointer(out x, out y);
			string key = Gdk.keyval_name(event.keyval);

			switch (key) {
				case "z":
					renderer.zoom(x, y, 1.1);
					break;
				case "x":
					renderer.zoom(x, y, 1.0/1.1);
					break;
				case "space":
					toggle_paused();
					break;
				case "Return":
					do_single_step();
					break;
				case "Escape":
					shutdown();
					break;
				case "y":
					toggle_render_all_debug_lines();
					break;
				case "p":
					show_screenshot_dialog();
					break;
				case "f":
					toggle_show_fps();
					break;
				case "o":
					toggle_control_picked();
					break;
				case "v":
					toggle_follow_picked();
					break;
				case "e":
					show_picked_log();
					break;
				case "Left":
					renderer.view_pos = renderer.view_pos.add(Vector.vec2(-100, 0));
					break;
				case "Right":
					renderer.view_pos = renderer.view_pos.add(Vector.vec2(100, 0));
					break;
				case "Up":
					renderer.view_pos = renderer.view_pos.add(Vector.vec2(0, 100));
					break;
				case "Down":
					renderer.view_pos = renderer.view_pos.add(Vector.vec2(0, -100));
					break;
				case "S":
					try {
						renderer.load_shaders();
					} catch (ShaderError e) {
						GLib.warning("reloading shaders failed:\n%s", e.message);
					}
					break;
				case "D":
					renderer.dump_perf();
					break;
				case "C":
					renderer.perf = new RenderPerf();
					break;
				default:
					if (renderer.picked != null && renderer.picked.controlled) {
						tick_lock.lock();
						renderer.picked.control(key, true);
						tick_lock.unlock();
					}
					break;
			}

			return true;
		}

		private void toggle_paused() {
			paused = !paused;
		}

		private void do_single_step() {
			paused = false;
			single_step = true;
		}

		private void toggle_render_all_debug_lines() {
			renderer.render_all_debug_lines = !renderer.render_all_debug_lines;
		}

		private void toggle_show_fps() {
			show_fps = !show_fps;
		}

		private void toggle_follow_picked() {
			if (renderer.picked != null) {
				renderer.follow_picked = !renderer.follow_picked;
			}
		}

		private void toggle_control_picked() {
			if (renderer.picked != null) {
				if (!renderer.picked.controlled) {
					renderer.picked.control_begin();
				} else {
					renderer.picked.control_end();
				}
			}
		}

		private void menu_zoom_in() {
			renderer.zoom(drawing_area.allocation.width/2, drawing_area.allocation.height/2, 2);
		}

		private void menu_zoom_out() {
			renderer.zoom(drawing_area.allocation.width/2, drawing_area.allocation.height/2, 0.5);
		}

		private bool on_key_release_event(Widget widget, EventKey event) {
			string key = Gdk.keyval_name(event.keyval);
			if (renderer.picked != null && renderer.picked.controlled) {
				renderer.picked.control(key, false);
			}
			return true;
		}

		private void shutdown() {
			stop_game();
			Gtk.main_quit();
		}

		private void show_picked_log() {
			if (renderer.picked != null) {
				tick_lock.lock();
				var w = new LogWindow(this, renderer.picked);
				w.update();
				tick_lock.unlock();
				w.show();
			}
		}

		private bool on_button_press_event(Widget widget, EventButton event) {
			int x, y;
			widget.get_pointer(out x, out y);

			switch (event.button) {
				case 1:
					renderer.follow_picked = false;
					tick_lock.lock();
					renderer.pick(x,y);
					tick_lock.unlock();
					break;
				default:
					break;
			}

			return true;
		}

		private bool on_scroll_event(Widget widget, EventScroll event) {
			int x, y;
			get_pointer(out x, out y);

			if (event.direction == Gdk.ScrollDirection.UP) {
				renderer.zoom(x, y, 1.1);
			} else if (event.direction == Gdk.ScrollDirection.DOWN) {
				renderer.zoom(x, y, 1.0/1.1);
			}

			return true;
		}

		public void start_game(uint32 seed, ParsedScenario scn, string[] ais) {
			try {
				start_game_int(seed, scn, ais);
			} catch (Error e) {
				warning("Game initialization failed: %s", e.message);
				start_demo_game();
			}
		}

		public void start_game_int(uint32 seed, ParsedScenario scn, string[] ai_filenames) throws FileError, ScenarioLoadError, ThreadError {
			if (game != null) stop_game();
			AI[] ais = {};
			foreach (var filename in ai_filenames) {
				uint8[] code;
				FileUtils.get_data(filename, out code);
				var ai = new AI() { code = code, filename = filename };
				ais += ai;
			}
			game = new Game(seed, scn, ais);
			start_renderer(game, scn.initial_view_scale);
			game_state = GameState.RUNNING;
			ticker = Thread.create<void*>(this.run, true);
		}

		public void stop_game() {
			shutting_down = true;
			ticker.join();
			ticker = null;
			shutting_down = false;
			game = null;
			renderer = null;
		}

		public void start_renderer(Game game, double initial_view_scale) {
			renderer = new Renderer(game, initial_view_scale);
			GLContext glcontext = WidgetGL.get_gl_context(drawing_area);
			GLDrawable gldrawable = WidgetGL.get_gl_drawable(drawing_area);

			if (!gldrawable.gl_begin(glcontext))
				error("failed to get GL context");

			renderer.init();
			renderer.reshape(drawing_area.allocation.width, drawing_area.allocation.height);

			gldrawable.gl_end();
		}

		public void start_demo_game() {
			try {
				var scn = Scenario.parse(Resources.load("scenarios/demo1.json"));
				start_game_int(42, scn, { });
				game_state = GameState.DEMO;
			} catch (Error e) {
				error("Demo initialization failed: %s", e.message);
			}
		}

		public void configure_scenario(string scenario_filename) {
			try {
				string data;
				FileUtils.get_contents(scenario_filename, out data);
				var scn = Scenario.parse(data.data);
				var w = new NewGameWindow(scn);
				w.transient_for = this;
				w.start_game.connect(start_game);
				w.show();
			} catch (Error e) {
				var w = new MessageDialog(this, DialogFlags.MODAL, MessageType.ERROR, ButtonsType.OK,
				                          "Failed to parse scenario: %s", e.message);
				w.response.connect((src,id) => { src.destroy(); });
				w.show();
			}
		}
	}

	class NewGameWindow : Gtk.Dialog {
		private ParsedScenario scn;

		private Widget ok_button;
		private Entry seed_entry;
		private FileChooserButton[] ai_choosers;

		public NewGameWindow(ParsedScenario scn) {
			this.scn = scn;
			this.title = "New Game";
			this.has_separator = false;
			this.border_width = 5;
			set_default_size(350, 100);

			this.vbox.spacing = 10;

			var metadata_str = "Name: %s\nDescription: %s\n".printf(scn.name, scn.description);
			var metadata_label = new Label(metadata_str);
			this.vbox.pack_start(metadata_label, false, false, 0);

			this.vbox.pack_start(new Label("AIs:"), false, false, 0);
			this.ai_choosers = new FileChooserButton[4];
			var i = 0;
			foreach (ParsedTeam pteam in this.scn.user_teams) {
				var chooser_hbox = new Gtk.HBox(false, 5);
				var color = Gdk.Color() { red=pteam.color_red<<8, green=pteam.color_green<<8, blue=pteam.color_blue<<8 };
				var color_button = new Gtk.ColorButton.with_color(color);
				color_button.sensitive = false;
				chooser_hbox.pack_start(color_button, false, false, 0);
				chooser_hbox.pack_start(new Label(pteam.name + ":"), false, false, 0);
				var chooser = new FileChooserButton("AI", Gtk.FileChooserAction.OPEN);
				chooser.file_set.connect(on_ai_change);
				chooser.set_current_folder(Resources.path("examples"));
				try {
					chooser.add_shortcut_folder(Resources.path("examples"));
				} catch (GLib.Error e) {}
				this.ai_choosers[i++] = chooser;
				chooser_hbox.pack_start(chooser, true, true, 3);
				this.vbox.pack_start(chooser_hbox, false, false, 0);
			}

			var seed_hbox = new Gtk.HBox(false, 5);
			seed_hbox.pack_start(new Label("Seed:"));
			this.seed_entry = new Gtk.Entry();
			this.seed_entry.text = Random.int_range(0,1000).to_string();
			seed_hbox.pack_start(seed_entry, false, false, 0);
			this.vbox.pack_start(seed_hbox, false, false, 0);

			add_button(Gtk.Stock.CLOSE, ResponseType.CLOSE);
			this.ok_button = add_button(Gtk.Stock.OK, ResponseType.APPLY);
			this.ok_button.sensitive = 0 == this.scn.user_teams.length();

			this.response.connect(on_response);

			show_all();
		}

		private void on_ai_change() {
			var cnt = 0;
			var j = 0;
			for (j = 0; j < this.scn.user_teams.length(); j++) {
				if (ai_choosers[j].get_filename() != null) {
					cnt++;
				}
			}
			this.ok_button.sensitive = cnt == this.scn.user_teams.length();
		}

		private void on_response (Dialog source, int response_id) {
			switch (response_id) {
			case ResponseType.APPLY:
				var n = this.scn.user_teams.length();
				var ais = new string[n];
				for (var i = 0; i < n; i++) {
					ais[i] = ai_choosers[i].get_filename();
				}
				start_game(int.parse(seed_entry.text), scn, ais);
				destroy();
				break;
			case ResponseType.CLOSE:
				destroy();
				break;
			}
		}

		public signal void start_game(uint32 seed, ParsedScenario scn, string[] ais);
	}
}

int main(string[] args) {
	GLib.Intl.setlocale(LocaleCategory.ALL, "C");
	GLib.Environment.set_application_name(Config.PACKAGE_NAME);

	Resources.init(args[0]);

	try {
		Gtk.init_with_args(ref args, "[scenario [ai...]]", options, null);
	} catch (Error e) {
		print("%s\n", e.message);
		return 1;
	}

	Gtk.gl_init(ref args);

	if (!Thread.supported ()) {
		print("Cannot run without thread support.\n");
		return 1;
	}

	if (!ShipClass.load()) {
		print("Failed to load ship classes.\n");
		return 1;
	}

	MainWindow mainwin;
	try {
		mainwin = new MainWindow();
	} catch (Error e) {
		print("%s\n", e.message);
		return 1;
	}

	if (args.length <= 1) {
		mainwin.start_demo_game();
	} else {
		var scenario_filename = args[1];
		var ai_filenames = args[2:(args.length)];

		ParsedScenario scn;
		try {
			string data;
			FileUtils.get_contents(scenario_filename, out data);
			scn = Scenario.parse(data.data);
		} catch (Error e) {
			print("Failed to parse scenario: %s\n", e.message);
			return 1;
		}

		try {
			mainwin.start_game_int(opt_seed, scn, ai_filenames);
		} catch (Error e) {
			print("Failed to start game: %s\n", e.message);
			return 1;
		}
	}

	Gtk.main();

	return 0;
}
