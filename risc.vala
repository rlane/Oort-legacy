using Gtk;
using Gdk;

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

		public MainWindow() {
			this.title = "RISC";
			this.destroy.connect(Gtk.main_quit);
			set_reallocate_redraws(true);

			var vbox = new VBox(false, 0);
			vbox.pack_start(make_menubar(), false, false, 0);
			vbox.pack_start(make_drawing_area(), true, true, 0);
			add(vbox);
			show_all();

			RISC.game_init(42, "scenarios/furball.lua", { "examples/orbit.lua" });

			GLib.Timeout.add(31, tick);
		}

		private MenuBar make_menubar() {
			var menubar = new MenuBar();
			var b = new MenuBuilder();

			b.menu(menubar, "Game", parent => {
				b.leaf(parent, "New", () => { new_game(); });
				b.leaf(parent, "Quit", () => { Gtk.main_quit(); });
			});

			b.menu(menubar, "Help", parent => {
				b.leaf(parent, "About", show_about);
			});

			return menubar;
		}

		private DrawingArea make_drawing_area() {
			drawing_area = new DrawingArea();
			//drawing_area.set_size_request(1024, 768);

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
			var w = new NewGameWindow();
			w.transient_for = this;
			w.start_game.connect(start_game);
			w.show();
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

			if (!gldrawable.gl_begin(glcontext))
				return false;

			render_gl13(paused);

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
			get_pointer(out x, out y);

			switch (event.button) {
				case 1:
					print("picking\n");
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
			if (RISC.game_init(seed, scenario, ais) != 0) {
				error("initialization failed\n");
			}
		}
	}

	class NewGameWindow : Gtk.Dialog {
		private Widget ok_button;
		private FileChooserButton scenario_chooser;
		private FileChooserButton[] ai_choosers;

		public NewGameWindow() {
			this.title = "New Game";
			this.has_separator = false;
			this.border_width = 5;
			set_default_size(350, 100);

			this.ai_choosers = new FileChooserButton[4];
			this.vbox.spacing = 10;
			this.vbox.pack_start(new Label("Scenario:"), false, false, 0);
			scenario_chooser = new FileChooserButton("Select scenario", Gtk.FileChooserAction.OPEN);
			this.vbox.pack_start(scenario_chooser, false, false, 0);
			this.vbox.pack_start(new Label("AIs:"), false, false, 0);
			var i = 0;
			for (i = 0; i < 4; i++) {
				ai_choosers[i] = new FileChooserButton("AI", Gtk.FileChooserAction.OPEN);
				this.vbox.pack_start(ai_choosers[i], false, false, 0);
			}

			add_button(STOCK_CLOSE, ResponseType.CLOSE);
			this.ok_button = add_button(STOCK_OK, ResponseType.APPLY);
			this.ok_button.sensitive = false;

			this.response.connect(on_response);
			this.scenario_chooser.file_set.connect( () => {
					this.ok_button.sensitive = this.scenario_chooser.get_filename() != null;
			});

			show_all();
		}

		private void on_response (Dialog source, int response_id) {
			switch (response_id) {
			case ResponseType.APPLY:
				var n = 0;
				for (var i = 0; i < 4; i++) {
					if (ai_choosers[i].get_filename() != null) n++;
				}
				var ais = new string[n];
				for (var i = 0; i < n; i++) {
					ais[i] = ai_choosers[i].get_filename();
				}
				start_game(5, scenario_chooser.get_filename(), ais);
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

	var mainwin = new RISC.MainWindow();

	Gtk.main();

	return 0;
}
