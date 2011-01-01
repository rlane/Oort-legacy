using Gtk;
using Gdk;
using Lua;
using RISC;

namespace RISC {
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
		private unowned Team winner;
		private Renderer renderer;

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

			this.renderer = new Renderer();

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
			scenario_chooser.set_current_folder(data_path("scenarios"));
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
				Game.purge();
				Game.tick(1.0/32);
				Particle.tick();
				renderer.tick();

				if (game_state == GameState.RUNNING) {
					winner = Game.check_victory();
					if (winner != null) {
						game_state = GameState.FINISHED;
					}
				}
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

			renderer.reshape(widget.allocation.width, widget.allocation.height);

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

			renderer.render();
			
			switch (game_state) {
			case GameState.DEMO:
				RISC.GLUtil.color32((uint32)0xFFFFFFAA);
				RISC.GLUtil.printf(rect.width/2-12*9, rect.height-50, "Click Game/New to begin");
				break;
			case GameState.RUNNING:
				break;
			case GameState.FINISHED:
				RISC.GLUtil.color32((uint32)0xFFFFFFAA);
				RISC.GLUtil.printf(rect.width/2-4*20, rect.height-50, "%s is victorious", winner.name);
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

			renderer.init();
			renderer.reset();

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
					paused = !paused;
					break;
				case "Return":
					paused = false;
					single_step = true;
					break;
				case "y":
					renderer.render_all_debug_lines = !renderer.render_all_debug_lines;
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
					renderer.pick(x,y);
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

		public void start_game(int seed, ParsedScenario scn, string[] ais) {
			RISC.Game.shutdown();
			renderer.init();
			try {
				if (RISC.Game.init(seed, scn, ais) != 0) {
					warning("initialization failed\n");
					start_demo_game();
				} else {
					game_state = GameState.RUNNING;
				}
			} catch (FileError e) {
				warning("Game initialization failed: %s", e.message);
			}
		}

		public void start_demo_game() {
			try {
				var scn = Scenario.parse(data_path("scenarios/demo1.json"));
				start_game(42, scn, { });
				game_state = GameState.DEMO;
			} catch (FileError e) {
				error("Demo initialization failed: %s", e.message);
			}
		}

		public void configure_scenario(string scenario_filename) {
			var scn = Scenario.parse(scenario_filename);
			if (scn == null) {
				var w = new MessageDialog(this, DialogFlags.MODAL, MessageType.ERROR, ButtonsType.OK,
				                          "Failed to parse scenario");
				w.show();
			} else {
				var w = new NewGameWindow(scn);
				w.transient_for = this;
				w.start_game.connect(start_game);
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
				chooser.set_current_folder(data_path("examples"));
				try {
					chooser.add_shortcut_folder(data_path("examples"));
				} catch (GLib.Error e) {}
				this.ai_choosers[i++] = chooser;
				chooser_hbox.pack_start(chooser, true, true, 3);
				this.vbox.pack_start(chooser_hbox, false, false, 0);
			}

			var seed_hbox = new Gtk.HBox(false, 5);
			seed_hbox.pack_start(new Label("Seed:"));
			this.seed_entry = new Gtk.Entry();
			this.seed_entry.text = "0";
			seed_hbox.pack_start(seed_entry, false, false, 0);
			this.vbox.pack_start(seed_hbox, false, false, 0);

			add_button(STOCK_CLOSE, ResponseType.CLOSE);
			this.ok_button = add_button(STOCK_OK, ResponseType.APPLY);
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
				start_game(seed_entry.text.to_int(), scn, ais);
				destroy();
				break;
			case ResponseType.CLOSE:
				destroy();
				break;
			}
		}

		public signal void start_game(int seed, ParsedScenario scn, string[] ais);
	}
}

int main(string[] args) {
	Gtk.init(ref args);
	Gtk.gl_init(ref args);

	if (!Thread.supported ()) {
		error ("Cannot run without thread support.");
	}

	Paths.init(args[0]);
	print("using data from %s\n", Paths.resource_dir.get_path());

	var mainwin = new MainWindow();

	if (args.length <= 1) {
		mainwin.start_demo_game();
	} else {
		mainwin.start_game(0, Scenario.parse(args[1]), args[2:(args.length)]);
	}

	Gtk.main();

	Game.shutdown();

	return 0;
}
