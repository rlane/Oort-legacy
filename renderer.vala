using GL;
using Vector;

namespace RISC {
	class Renderer {
		Rand prng;

		public void init() {
			RISC.GL13.init();

			prng = new Rand();

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
			prng.set_seed(0);
			RISC.GL13.render(paused, render_all_debug_lines);

			foreach (unowned Bullet b in RISC.all_bullets) {
				render_bullet(b);
			}
		}

		private void render_bullet(Bullet b) {
			if (b.dead) return;

			if (b.type == RISC.BulletType.SLUG) {
				var dp = b.physics.v.scale(1.0/64);
				var offset = b.physics.v.scale(prng.next_double()/64);
				var p1 = b.physics.p.add(offset);
				var p2 = b.physics.p.add(offset).add(dp);
				var sp1 = RISC.GL13.S(p1);
				var sp2 = RISC.GL13.S(p2);

				glBegin(GL_LINE_STRIP);
				RISC.GLUtil.color32(0x44444455);
				glVertex3d(sp1.x, sp1.y, 0);
				RISC.GLUtil.color32(0x444444FF);
				glVertex3d(sp2.x, sp2.y, 0);
				glEnd();
			}
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

			foreach (unowned Bullet b in RISC.all_bullets) {
				if (b.type == BulletType.PLASMA) {
					Particle.shower(ParticleType.PLASMA, b.physics.p, vec2(0,0), b.physics.v.scale(1/63),
							            double.min(b.physics.m/5,0.1), 3, 4, 6);
				}
			}

			foreach (unowned BulletHit hit in RISC.bullet_hits) {
				Particle.shower(ParticleType.HIT, hit.cp, hit.s.physics.v.scale(1/32), vec2(0,0), 0.1, 1, 20, (uint16)(hit.e*100));
			}
		}
	}

	namespace GLUtil {
		public void printf(int x, int y, string fmt, ...) {
			va_list ap = va_list();
			RISC.GL13.vprintf(x, y, fmt, ap);
		}

		public void color32(uint32 c) {
			GLubyte r = (GLubyte) ((c >> 24) & 0xFF);
			GLubyte g = (GLubyte) ((c >> 16) & 0xFF);
			GLubyte b = (GLubyte) ((c >> 8) & 0xFF);
			GLubyte a = (GLubyte) (c & 0xFF);
			glColor4ub(r, g, b, a);
		}
	}
}
