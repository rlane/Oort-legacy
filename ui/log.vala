using Gtk;

namespace Oort {
	class LogWindow : Gtk.Window {
		public unowned Ship ship;
		Game game;
		Gtk.TextView text;
		Gtk.ScrolledWindow scroll;
		int cursor = 0;
		MainWindow main;
		ulong attachment;

		public LogWindow(MainWindow main, Ship s) {
			this.title = "Log";
			ship = s;
			game = s.game;
			text = new Gtk.TextView();
			text.editable = false;
			text.cursor_visible = false;
			scroll = new Gtk.ScrolledWindow(null, null);
			scroll.add(text);
			add(scroll);
			show_all();
			this.main = main;
			attachment = main.ticked.connect((t) => update());
		}

		public void update() {
			var buf = text.get_buffer();
			Gtk.TextIter iter;
			if (ship == null) {
				return;
			} else if (ship != null && ship.dead) {
				ship = null;
				buf.get_end_iter(out iter);
				buf.insert(iter, "(dead)", -1);
			} else {
				var head = ship.get_logbuf_head();
				for (; cursor <= head; cursor++) {
					var e = ship.get_logbuf_entry(cursor);
					if (e == null) {
						continue;
					}
					buf.get_end_iter(out iter);
					buf.insert(iter, e, -1);
					buf.get_end_iter(out iter);
					buf.insert(iter, "\n", 1);
				}
			}
		}

		~LogWindow() {
			main.disconnect(attachment);
		}
	}
}
