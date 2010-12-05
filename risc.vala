using Gtk;
using Gdk;
using Lua;

namespace RISC {
	struct ScenarioMetadata {
		public string filename;
		public string name;
		public string author;
		public string version;
		public string description;
		public int min_teams;
		public int max_teams;
	}

	class MenuBuilder : GLib.Object {
		public delegate void MenuAction();
		public void leaf(MenuShell parent, string label, MenuAction action) {
			var item = new MenuItem.with_label(label);
			parent.append(item);
			item.activate.connect((widget) => action());
		}

		public delegate void MenuBuilder(MenuShell parent);
		public void menu(MenuShell parent, string label, MenuBuilder builder) {
			var item = new MenuItem.with_label(label);
			var menu = new Menu();
			item.set_submenu(menu);
			parent.append(item);
			builder(menu);
		}
	}

	class MainWindow : Gtk.Window {
		private DrawingArea drawing_area;
		private bool paused;
		private bool single_step;

		enum GameState {
			DEMO,
			RUNNING,
			FINISHED,
		}

		private GameState game_state;

		public MainWindow() {
			this.title = "RISC";
			this.destroy.connect(Gtk.main_quit);
			set_reallocate_redraws(true);

			var vbox = new VBox(false, 0);
			vbox.pack_start(make_menubar(), false, false, 0);
			vbox.pack_start(make_drawing_area(), true, true, 0);
			add(vbox);
			show_all();

			GLib.Timeout.add(31, tick);
		}

		private MenuBar make_menubar() {
			var menubar = new MenuBar();
			var b = new MenuBuilder();

			b.menu(menubar, "Game", parent => {
				b.leaf(parent, "New", () => { new_game(); });
				b.leaf(parent, "Stop", () => { start_demo_game(); });
				b.leaf(parent, "Quit", () => { Gtk.main_quit(); });
			});

			b.menu(menubar, "Help", parent => {
				b.leaf(parent, "About", show_about);
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
			drawing_area.button_press_event.connect(on_button_press_event);
			drawing_area.scroll_event.connect(on_scroll_event);

			return drawing_area;
		}

		public void new_game() {
			var scenario_chooser = new FileChooserDialog("Select scenario", this, Gtk.FileChooserAction.OPEN,
			                                             Gtk.STOCK_OK, Gtk.ResponseType.ACCEPT,
																									 Gtk.STOCK_CANCEL, Gtk.ResponseType.REJECT);
			try {
				scenario_chooser.add_shortcut_folder(data_path("scenarios"));
			} catch (GLib.Error e) {}
			scenario_chooser.response.connect( (response_id) => {
				if (response_id == Gtk.ResponseType.ACCEPT) {
					configure_scenario(scenario_chooser.get_filename());
				}
				scenario_chooser.destroy();
			});
			scenario_chooser.show();
		}

		public void show_about() {
			var w = new AboutDialog();
			w.transient_for = this;
			w.authors = { "Rich Lane", null };
			w.version = "alpha";
			w.response.connect( (response_id) => { w.destroy(); });
			w.show();
		}

		private bool tick() {
			if (!paused) {
				game_purge();
				game_tick(1.0/32);
				particle_tick();
				emit_particles();

/*
				struct team *winner;
				if ((winner = game_check_victory())) {
					printf("Team '%s' is victorious in %0.2f seconds\n", winner->name, ticks*tick_length);
					paused = 1;
				}
*/
			}

			if (single_step) {
				paused = true;
				single_step = false;
			}

			var window = drawing_area.window;
			window.invalidate_rect((Rectangle)drawing_area.allocation, false);

			return true;
		}

		/* Widget is resized */
		private bool on_configure_event(Widget widget, EventConfigure event) {
			GLContext glcontext = WidgetGL.get_gl_context(widget);
			GLDrawable gldrawable = WidgetGL.get_gl_drawable(widget);

			if (!gldrawable.gl_begin(glcontext))
				return false;

			reshape_gl13(widget.allocation.width, widget.allocation.height);

			gldrawable.gl_end();
			return true;
		}

		/* Widget is asked to paint itself */
		private bool on_expose_event(Widget widget, EventExpose event) {
			GLContext glcontext = WidgetGL.get_gl_context(widget);
			GLDrawable gldrawable = WidgetGL.get_gl_drawable(widget);

			var rect = drawing_area.allocation;

			if (!gldrawable.gl_begin(glcontext))
				return false;

			render_gl13(paused);
			
			switch (game_state) {
			case GameState.DEMO:
				glColor32((uint32)0xFFFFFFAA);
				glPrintf(rect.width/2-12*9, rect.height-50, "Click Game/New to begin");
				break;
			case GameState.RUNNING:
				break;
			case GameState.FINISHED:
				glColor32((uint32)0xFFFFFFAA);
				glPrintf(rect.width/2-4*9, rect.height-50, "Game Over");
				break;
			}

			gldrawable.swap_buffers();

			gldrawable.gl_end();
			return true;
		}

		private void on_realize_event(Widget widget) {
			GLContext glcontext = WidgetGL.get_gl_context(widget);
			GLDrawable gldrawable = WidgetGL.get_gl_drawable(widget);

			if (!gldrawable.gl_begin(glcontext))
				return;

			init_gl13();
			reset_gl13();

			gldrawable.gl_end();
		}

		private bool on_key_press_event(Widget widget, EventKey event) {
			int x, y;
			get_pointer(out x, out y);
			string key = Gdk.keyval_name(event.keyval);

			switch (key) {
				case "z":
					zoom(x, y, 1.1);
					break;
				case "x":
					zoom(x, y, 1.0/1.1);
					break;
				case "space":
					paused = !paused;
					break;
				case "Return":
					paused = false;
					single_step = true;
					break;
				case "y":
					//render_all_debug_lines = !render_all_debug_lines;
					break;
				case "p":
					screenshot("screenshot.tga");
					break;
				case "Escape":
					Gtk.main_quit();
					break;
			}

			return true;
		}

		private bool on_button_press_event(Widget widget, EventButton event) {
			int x, y;
			widget.get_pointer(out x, out y);

			switch (event.button) {
				case 1:
					pick(x,y);
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
				zoom(x, y, 1.1);
			} else if (event.direction == Gdk.ScrollDirection.DOWN) {
				zoom(x, y, 1.0/1.1);
			}

			return true;
		}

		public void start_game(int seed, string scenario, string[] ais) {
			RISC.game_shutdown();
			reset_gl13();
			if (RISC.game_init(seed, scenario, ais) != 0) {
				warning("initialization failed\n");
				start_demo_game();
			} else {
				game_state = GameState.RUNNING;
			}
		}

		public void start_demo_game() {
			start_game(42, data_path("scenarios/demo1.lua"), { });
			game_state = GameState.DEMO;
		}

		delegate string? GetStr(string k);

		public void configure_scenario(string scenario_filename) {
			var L = new Lua.LuaVM();
			L.open_libs();
			L.push_string(scenario_filename);
			L.set_global("filename");
			if (L.do_file(data_path("scenario_parser.lua"))) {
				stderr.printf("Failed to parse scenario: %s\n", L.to_string(-1));
				return;
			}

			var errors = 0;

			GetStr getstr = (k) => {
				L.get_field(-1, k);
				if (L.is_string(-1)) {
					var v = L.to_string(-1);
					L.pop(1);
					return v;
				} else {
					L.pop(1);
					stderr.printf("missing field %s\n", k);
					errors++;
					return null;
				}
			};

			var scn = ScenarioMetadata();
			scn.filename = scenario_filename;
			scn.name = getstr("name");
			scn.author = getstr("author");
			scn.version = getstr("version");
			scn.description = getstr("description");
			string min_teams_str = getstr("min_teams");
			string max_teams_str = getstr("max_teams");

			if (errors > 0) {
				stderr.printf("missing required fields\n");
				return;
			}

			scn.min_teams = min_teams_str.to_int();
			scn.max_teams = max_teams_str.to_int();

			var w = new NewGameWindow(scn);
			w.transient_for = this;
			w.start_game.connect(start_game);
			w.show();
		}
	}

	class NewGameWindow : Gtk.Dialog {
		private ScenarioMetadata scn;

		private Widget ok_button;
		private Entry seed_entry;
		private FileChooserButton[] ai_choosers;

		public NewGameWindow(ScenarioMetadata scn) {
			this.scn = scn;
			this.title = "New Game";
			this.has_separator = false;
			this.border_width = 5;
			set_default_size(350, 100);

			this.vbox.spacing = 10;

			var metadata_str = "Name: %s\nDescription: %s\nTeams: %d to %d\n".printf(scn.name, scn.description, scn.min_teams, scn.max_teams);
			var metadata_label = new Label(metadata_str);
			this.vbox.pack_start(metadata_label, false, false, 0);

			this.vbox.pack_start(new Label("AIs:"), false, false, 0);
			this.ai_choosers = new FileChooserButton[4];
			var i = 0;
			for (i = 0; i < this.scn.max_teams; i++) {
				var chooser = new FileChooserButton("AI", Gtk.FileChooserAction.OPEN);
				chooser.file_set.connect(on_ai_change);
				try {
					chooser.add_shortcut_folder(data_path("examples"));
				} catch (GLib.Error e) {}
				this.ai_choosers[i] = chooser;
				this.vbox.pack_start(chooser, false, false, 0);
			}

			var seed_hbox = new Gtk.HBox(false, 5);
			seed_hbox.pack_start(new Label("Seed:"));
			this.seed_entry = new Gtk.Entry();
			this.seed_entry.text = "0";
			seed_hbox.pack_start(seed_entry, false, false, 0);
			this.vbox.pack_start(seed_hbox, false, false, 0);

			add_button(STOCK_CLOSE, ResponseType.CLOSE);
			this.ok_button = add_button(STOCK_OK, ResponseType.APPLY);
			this.ok_button.sensitive = false;

			this.response.connect(on_response);

			show_all();
		}

		private void on_ai_change() {
			var cnt = 0;
			var j = 0;
			for (j = 0; j < this.scn.max_teams; j++) {
				if (ai_choosers[j].get_filename() != null) {
					cnt++;
				}
			}
			this.ok_button.sensitive = cnt >= this.scn.min_teams;
		}

		private void on_response (Dialog source, int response_id) {
			switch (response_id) {
			case ResponseType.APPLY:
				var n = 0;
				for (var i = 0; i < this.scn.max_teams; i++) {
					if (ai_choosers[i].get_filename() != null) n++;
				}
				var ais = new string[n];
				for (var i = 0; i < n; i++) {
					ais[i] = ai_choosers[i].get_filename(); // XXX
				}
				start_game(seed_entry.text.to_int(), scn.filename, ais);
				destroy();
				break;
			case ResponseType.CLOSE:
				destroy();
				break;
			}
		}

		public signal void start_game(int seed, string scenario, string[] ais);
	}
}

int main(string[] args) {
	Gtk.init(ref args);
	Gtk.gl_init(ref args);

	if (!Thread.supported ()) {
		error ("Cannot run without thread support.");
	}

	if (!RISC.find_data_dir()) {
		error("could not find data dir (set RISC_DATA)");
	}

	var mainwin = new RISC.MainWindow();

	if (args.length <= 1) {
		mainwin.start_demo_game();
	} else {
		mainwin.start_game(0, args[1], args[2:(args.length)]);
	}

	Gtk.main();

	return 0;
}
