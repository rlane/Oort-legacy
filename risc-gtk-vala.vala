using Gtk;
using Gdk;

namespace RISC {

	class MainWindow : Gtk.Window {
		private DrawingArea drawing_area;
		private bool paused;
		private bool single_step;

		public MainWindow() {
			this.title = "RISC";
			this.destroy.connect(Gtk.main_quit);
			set_reallocate_redraws(true);

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

			add(drawing_area);

			GLib.Timeout.add(31, tick);
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
	}

}

int main(string[] args) {
	Gtk.init(ref args);
	Gtk.gl_init(ref args);

	int seed = 5;

	if (!Thread.supported ()) {
		error ("Cannot run without thread support.");
	}

	if (RISC.game_init(seed, null, 0, null) != 0) {
		error("initialization failed\n");
	}

	var sample = new RISC.MainWindow();
	sample.show_all();

	Gtk.main();

	return 0;
}
