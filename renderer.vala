using GL;

namespace RISC {
	class Renderer {
		public void init() {
			RISC.GL13.init();

			glEnable(GL_TEXTURE_2D);
			glClearColor(0.0f, 0.0f, 0.03f, 0.0f);
			glShadeModel(GL_SMOOTH);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);
			glLineWidth(1.2f);

			/*
			print("Vendor: %s\n", glGetString(GL_VENDOR));
			print("Renderer: %s\n", glGetString(GL_RENDERER));
			print("GL Version: %s\n", glGetString(GL_VERSION));
			//print("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
			print("Extensions:\n%s\n", glGetString(GL_EXTENSIONS));
			*/
		}

		public void reset() {
			RISC.GL13.reset();
		}

		public void render(bool paused, bool render_all_debug_lines) {
			RISC.GL13.render(paused, render_all_debug_lines);
		}

		public void reshape(int width, int height) {
			RISC.GL13.reshape(width, height);
			glViewport (0, 0, (GLsizei)width, (GLsizei)height);
			glMatrixMode (GL_PROJECTION);
			glLoadIdentity ();
			glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
			glMatrixMode (GL_MODELVIEW);
			glLoadIdentity ();
		}

		public void zoom(int x, int y, double force) {
			RISC.GL13.zoom(x, y, force);
		}

		public void pick(int x, int y) {
			RISC.GL13.pick(x, y);
		}

		public void tick() {
			RISC.GL13.emit_particles();
		}

		public void glPrintf(int x, int y, string fmt, ...) {
			va_list ap = va_list();
			RISC.GL13.vprintf(x, y, fmt, ap);
		}

		public void glColor32(uint32 c) {
			RISC.GL13.glColor32(c);
		}
	}
}
