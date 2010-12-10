using GL;
using Vector;
using Math;

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

			foreach (unowned Ship s in RISC.all_ships) {
				render_ship(s);
			}

			foreach (unowned Bullet b in RISC.all_bullets) {
				render_bullet(b);
			}

			render_particles();
			
			if (GL13.picked != null) {
				render_picked_info(GL13.picked);
			}
		}

		void triangle_fractal(int depth) {
			double alt = 0.8660254;

			if (depth > 1) {
				glBegin(GL_LINES);
				glVertex3d(alt, 0, 0);
				glVertex3d(3*alt/4, -0.125, 0);
				glVertex3d(alt/4, -0.375, 0);
				glVertex3d(0, -0.5, 0);
				glVertex3d(alt, 0, 0);
				glVertex3d(3*alt/4, 0.125, 0);
				glVertex3d(alt/4, 0.375, 0);
				glVertex3d(0, 0.5, 0);
				glEnd();

				glPushMatrix();
				glScaled(0.5, 0.5, 0.5);
				glRotated(60, 0, 0, 1);
				glTranslated(alt, -0.5, 0);
				triangle_fractal(depth-1);
				glPopMatrix();

				glPushMatrix();
				glScaled(0.5, 0.5, 0.5);
				glRotated(-60, 0, 0, 1);
				glTranslated(alt, 0.5, 0);
				triangle_fractal(depth-1);
				glPopMatrix();
			} else {
				glBegin(GL_LINE_STRIP);
				glVertex3d(0, -0.5, 0);
				glVertex3d(alt, 0, 0);
				glVertex3d(0, 0.5, 0);
				glEnd();
			}
		}

		void render_mothership(Ship s) {
			int depth = int.min(int.max((int)Math.log2(GL13.view_scale), 2), 8);
			GLUtil.color32(s.team.color | 0xEE);
			glPushMatrix();
			glScaled(0.5, 0.3, 0.3);
			GLUtil.render_circle(5);
			glPopMatrix();
			triangle_fractal(depth);
			glPushMatrix();
			glRotated(180, 0, 0, 1);
			triangle_fractal(depth);
			glPopMatrix();
		}

		void render_fighter(Ship s) {
			GLUtil.color32(s.team.color | 0xAA);
			glBegin(GL_LINE_LOOP);
			glVertex3d(-0.70, -0.71, 0);
			glVertex3d(-0.70, 0.71, 0);
			glVertex3d(1, 0, 0);
			glEnd();
		}

		void render_missile(Ship s) {
			GLUtil.color32((uint32)0x88888800 | 0x55);
			GLUtil.render_circle(5);
		}

		void render_little_missile(Ship s) {
			GLUtil.color32((uint32)0x88888800 | 0x55);
			glBegin(GL_LINES);
			glVertex3d(-0.70, -0.71, 0);
			glVertex3d(-0.2, 0, 0);
			glVertex3d(-0.70, 0.71, 0);
			glVertex3d(-0.2, 0, 0);
			glVertex3d(-0.2, 0, 0);
			glVertex3d(1, 0, 0);
			glEnd();
		}

		void render_unknown(Ship s) {
			GLUtil.color32((uint32)0x88888800 | 0x55);
			GLUtil.render_circle(8);
		}

		void render_ship(Ship s) {
			var sp = GL13.S(s.physics.p);
			double angle = s.gfx.angle;
			double scale = GL13.view_scale * s.class.radius;

			glPushMatrix();
			glTranslated(sp.x, sp.y, 0);
			glScaled(scale, scale, scale);
			glRotated(rad2deg(angle), 0, 0, 1);

			if (s.class.name == "mothership") {
				render_mothership(s);
			} else if (s.class.name == "fighter") {
				render_fighter(s);
			} else if (s.class.name == "missile") {
				render_missile(s);
			} else if (s.class.name == "little_missile") {
				render_little_missile(s);
			} else {
				render_unknown(s);
			}

			glPopMatrix();

			int tail_alpha_max = s.class.name.contains("missile") ? 16 : 64;
			glBegin(GL_LINE_STRIP);
			GLUtil.color32(s.team.color | tail_alpha_max);
			glVertex3d(sp.x, sp.y, 0);
			int i;
			for (i = 0; i < Ship.TAIL_SEGMENTS-1; i++) {
				int j = s.tail_head - i - 1;
				if (j < 0) j += Ship.TAIL_SEGMENTS;
				Vec2 sp2 = GL13.S(s.tail[j]);
				if (isnan(sp2.x) != 0)
					break;
				uint32 color = s.team.color | (tail_alpha_max-(tail_alpha_max/Ship.TAIL_SEGMENTS)*i);

				GLUtil.color32(color);
				glVertex3d(sp2.x, sp2.y, 0);
			}
			glEnd();

			if (s == GL13.picked) {
				GLUtil.color32((uint32)0xCCCCCCAA);
				glPushMatrix();
				glTranslated(sp.x, sp.y, 0);
				glScaled(scale, scale, scale);
				GLUtil.render_circle(64);
				glPopMatrix();

				GLUtil.color32((uint32)0xCCCCCC77);
				glPushMatrix();
				glTranslated(sp.x, sp.y, 0);
				glScaled(GL13.view_scale, GL13.view_scale, GL13.view_scale);
				glBegin(GL_LINES);
				glVertex3d(0, 0, 0);
				glVertex3d(s.physics.thrust.x, s.physics.thrust.y, 0);
				glEnd();
				glPopMatrix();

				GLUtil.color32((uint32)0x49D5CEAA);
				glBegin(GL_LINE_STRIP);
				glVertex3d(sp.x, sp.y, 0);
				Physics q = s.physics.copy();
				double tick_length = 1/32.0;
				for (double j = 0; j < 1/tick_length; j++) {
					q.tick_one(&tick_length);
					Vec2 sp2 = GL13.S(q.p);
					glVertex3d(sp2.x, sp2.y, 0);
				}
				glEnd();
			}

			if (s == GL13.picked || GL13.render_all_debug_lines) {
				GLUtil.color32((uint32)0x49D5CEAA);
				glBegin(GL_LINES);
				for (int j = 0; j < s.debug.num_lines; j++) {
					Vec2 sa = GL13.S(s.debug.lines[j].a);
					Vec2 sb = GL13.S(s.debug.lines[j].b);
					glVertex3d(sa.x, sa.y, 0);
					glVertex3d(sb.x, sb.y, 0);
				}
				glEnd();
			}

			// XXX move
			if (s == GL13.picked && s.dead) {
				GL13.picked = null;
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

		private void render_particles() {
			for (int i = 0; i < Particle.MAX; i++) {
				unowned Particle c = Particle.get(i);
				if (c.ticks_left == 0) continue;
				Vec2 p = GL13.S(c.p);
				if (c.type == ParticleType.HIT) {
					glPointSize((float)(0.3*c.ticks_left*GL13.view_scale/32));
					glColor4ub(255, 200, 200, c.ticks_left*8);
				} else if (c.type == ParticleType.PLASMA) {
					glPointSize((float)(0.15*c.ticks_left*GL13.view_scale/32));
					glColor4ub(255, 0, 0, c.ticks_left*32);
				} else if (c.type == ParticleType.ENGINE) {
					glPointSize((float)(0.1*c.ticks_left*GL13.view_scale/32));
					glColor4ub(255, 217, 43, 10 + c.ticks_left*5);
				}
				glBegin(GL_POINTS);
				glVertex3d(p.x, p.y, 0);
				glEnd();
			}
		}

		private void render_picked_info(Ship s) {
			int x = 15;
			int y = 82;
			int dy = 12;
			GLUtil.color32((uint32)0xAAFFFFAA);
			GLUtil.printf(x, y-0*dy, "%s %.8x", s.class.name, s.api_id);
			GLUtil.printf(x, y-1*dy, "hull: %.2f", s.hull);
			GLUtil.printf(x, y-2*dy, @"position: $(s.physics.p)");
			GLUtil.printf(x, y-3*dy, @"velocity: $(s.physics.v) $(s.physics.v.abs())");
			GLUtil.printf(x, y-4*dy, @"thrust: $(s.physics.thrust) $(s.physics.thrust.abs())");
			GLUtil.printf(x, y-5*dy, "energy: %g", s.get_energy());
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
			foreach (unowned Bullet b in RISC.all_bullets) {
				if (b.type == BulletType.PLASMA) {
					Particle.shower(ParticleType.PLASMA, b.physics.p, vec2(0,0), b.physics.v.scale(1/63),
							            double.min(b.physics.m/5,0.1), 3, 4, 6);
				}
			}

			foreach (unowned BulletHit hit in RISC.bullet_hits) {
				Particle.shower(ParticleType.HIT, hit.cp, hit.s.physics.v.scale(1/32), vec2(0,0), 0.1, 1, 20, (uint16)(hit.e*100));
			}

			foreach (unowned Ship s in RISC.all_ships) {
				if (s.physics.thrust.abs() != 0) {
					Particle.shower(ParticleType.ENGINE, s.physics.p, s.physics.v.scale(1/32), s.physics.thrust.scale(-1/32), 0.1, 1, 4, 8);
				}

				double v_angle = atan2(s.physics.v.y, s.physics.v.x); // XXX reversed?
				double da = normalize_angle(v_angle - s.gfx.angle);
				s.gfx.angle = normalize_angle(s.gfx.angle + s.gfx.class.rotfactor*da);
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

		public void render_circle(int n)
		{
			double da = 2*Math.PI/n, a = 0;
			int i;

			glBegin(GL_LINE_LOOP);
			for (i = 0; i < n; i++) {
				a += da;
				glVertex3d(cos(a), sin(a), 0);
			}
			glEnd();
		}
	}

	public double normalize_angle(double a)
	{
		if (a < -PI) a += 2*PI;
		if (a > PI) a -= 2*PI;
		return a;
	}
}
