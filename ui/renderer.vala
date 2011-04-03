using GL;
using Vector;
using Math;

[Compact]
public class RISC.ShipGfxClass {
	public static ShipGfxClass fighter;
	public static ShipGfxClass ion_cannon_frigate;
	public static ShipGfxClass mothership;
	public static ShipGfxClass missile;
	public static ShipGfxClass little_missile;
	public static ShipGfxClass unknown;
	
	public static void init() {
		fighter = new ShipGfxClass();
		ion_cannon_frigate = new ShipGfxClass();
		mothership = new ShipGfxClass();
		missile = new ShipGfxClass();
		little_missile = new ShipGfxClass();
		unknown = new ShipGfxClass();
	}

	public static unowned ShipGfxClass lookup(string name)
	{
		switch (name) {
			case "fighter": return fighter;
			case "ion_cannon_frigate": return ion_cannon_frigate;
			case "mothership": return mothership;
			case "missile": return missile;
			case "little_missile": return little_missile;
			default: return unknown;
		}
	}
}

namespace RISC {
	class Renderer {
		public bool render_all_debug_lines = false;
		public int screen_width = 640;
		public int screen_height = 480;
		public double view_scale;
		public Vec2 view_pos;
		public unowned Ship picked = null;
		public Game game;
		public bool render_explosion_rays = false;
		public bool follow_picked = false;

		Rand prng;

		public static void static_init() {
			if (GLEW.init()) {
				error("GLEW initialization failed");
			}
			ShipGfxClass.init();
			RISC.Ship.gfx_create_cb = on_ship_created;

			/*
			print("Vendor: %s\n", glGetString(GL_VENDOR));
			print("Renderer: %s\n", glGetString(GL_RENDERER));
			print("GL Version: %s\n", glGetString(GL_VERSION));
			//print("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
			print("Extensions:\n%s\n", glGetString(GL_EXTENSIONS));
			*/
		}

		public Renderer(Game game, double initial_view_scale) {
			this.game = game;
			view_scale = initial_view_scale;
			prng = new Rand();
			view_pos = vec2(0,0);
		}

		public void init() {
			glEnable(GL_TEXTURE_2D);
			glClearColor(0.0f, 0.0f, 0.03f, 0.0f);
			glShadeModel(GL_SMOOTH);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);
			glLineWidth(1.2f);
		}

		public void render() {
			prng.set_seed(0); // XXX tick seed

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();

			if (follow_picked && picked != null) {
				view_pos = picked.physics.p;
			}

			foreach (unowned Ship s in game.all_ships) {
				render_ship(s);
			}

			foreach (unowned Bullet b in game.all_bullets) {
				render_bullet(b);
			}

			render_particles();
			
			if (picked != null) {
				render_picked_info(picked);
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
			int depth = int.min(int.max((int)Math.log2(view_scale*100), 2), 8);
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

		void render_ion_cannon_frigate(Ship s) {
			GLUtil.color32(s.team.color | 0xBB);
			glBegin(GL_LINE_LOOP);
			glVertex3d(-0.80, -0.3, 0);
			glVertex3d(-0.80, 0.3, 0);
			glVertex3d(0.95, 0.2, 0);
			glVertex3d(0.95, 0.08, 0);
			glVertex3d(0.7, 0.08, 0);
			glVertex3d(0.7, -0.08, 0);
			glVertex3d(0.95, -0.08, 0);
			glVertex3d(0.95, -0.2, 0);
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
			var sp = S(s.physics.p);
			double angle = s.physics.h;
			double scale = view_scale * s.class.radius;

			glPushMatrix();
			glTranslated(sp.x, sp.y, 0);
			glScaled(scale, scale, scale);
			glRotated(Util.rad2deg(angle), 0, 0, 1);

			// XXX move into class
			if (s.class.name == "mothership") {
				render_mothership(s);
			} else if (s.class.name == "fighter") {
				render_fighter(s);
			} else if (s.class.name == "ion_cannon_frigate") {
				render_ion_cannon_frigate(s);
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
				Vec2 sp2 = S(s.tail[j]);
				if (isnan(sp2.x) != 0)
					break;
				uint32 color = s.team.color | (tail_alpha_max-(tail_alpha_max/Ship.TAIL_SEGMENTS)*i);

				GLUtil.color32(color);
				glVertex3d(sp2.x, sp2.y, 0);
			}
			glEnd();

			if (s == picked) {
				GLUtil.color32((uint32)0xCCCCCCAA);
				glPushMatrix();
				glTranslated(sp.x, sp.y, 0);
				glScaled(scale, scale, scale);
				GLUtil.render_circle(64);
				glPopMatrix();

				GLUtil.color32((uint32)0xCCCCCC77);
				glPushMatrix();
				glTranslated(sp.x, sp.y, 0);
				glScaled(view_scale, view_scale, view_scale);
				glRotated(Util.rad2deg(s.physics.h), 0, 0, 1);
				glBegin(GL_LINES);
				glVertex3d(0, 0, 0);
				glVertex3d(s.physics.acc.x, s.physics.acc.y, 0);
				glEnd();
				glPopMatrix();

				GLUtil.color32((uint32)0x49D5CEAA);
				glBegin(GL_LINE_STRIP);
				glVertex3d(sp.x, sp.y, 0);
				Physics q = s.physics.copy();
				for (double j = 0; j < 1/Game.TICK_LENGTH; j++) {
					q.tick_one();
					Vec2 sp2 = S(q.p);
					glVertex3d(sp2.x, sp2.y, 0);
				}
				glEnd();
			}

			if (s == picked || render_all_debug_lines) {
				GLUtil.color32((uint32)0x49D5CEAA);
				glBegin(GL_LINES);
				for (int j = 0; j < s.debug.num_lines; j++) {
					Vec2 sa = S(s.debug.lines[j].a);
					Vec2 sb = S(s.debug.lines[j].b);
					glVertex3d(sa.x, sa.y, 0);
					glVertex3d(sb.x, sb.y, 0);
				}
				glEnd();
			}

			// XXX move
			if (s == picked && s.dead) {
				picked = null;
			}
		}

		private void render_bullet(Bullet b) {
			RISC.GLUtil.color32((uint32)0xFFFFFFAA);

			if (b.dead) return;

			if (b.type == RISC.BulletType.SLUG) {
				var dp = b.physics.v.scale(1.0/64);
				var offset = b.physics.v.scale(prng.next_double()/64);
				var p1 = b.physics.p.add(offset);
				var p2 = b.physics.p.add(offset).add(dp);
				var sp1 = S(p1);
				var sp2 = S(p2);

				glBegin(GL_LINE_STRIP);
				RISC.GLUtil.color32(0x44444455);
				glVertex3d(sp1.x, sp1.y, 0);
				RISC.GLUtil.color32(0x444444FF);
				glVertex3d(sp2.x, sp2.y, 0);
				glEnd();
			} else if (b.type == RISC.BulletType.ION_BEAM) {
				var sp = S(b.physics.p);
				var angle = Math.atan2(b.physics.v.y, b.physics.v.x);
				var length = b.physics.v.distance(vec2(0,0)) * Game.TICK_LENGTH;
				var width = 3;
				glPushMatrix();
				glTranslated(sp.x, sp.y, 0);
				glRotated(Util.rad2deg(angle), 0, 0, 1);
				glScaled(view_scale, view_scale, view_scale);
				glBegin(GL_QUADS);
				RISC.GLUtil.color32(0x6464FFAA);
				glVertex3d(0.7*40, width, 0);
				glVertex3d(0.7*40, -width, 0);
				glVertex3d(length, -width, 0);
				glVertex3d(length, width, 0);
				glEnd();
				glPopMatrix();
			} else if (render_explosion_rays && b.type == RISC.BulletType.EXPLOSION) {
				var dp = b.physics.v.scale(Game.TICK_LENGTH);
				var sp1 = S(b.physics.p);
				var sp2 = S(b.physics.p.add(dp));

				glBegin(GL_LINE_STRIP);
				RISC.GLUtil.color32(0xFFFFFF33u);
				glVertex3d(sp1.x, sp1.y, 0);
				RISC.GLUtil.color32(0xFFFFFF22u);
				glVertex3d(sp2.x, sp2.y, 0);
				glEnd();
			}
		}

		private void render_particles() {
			for (int i = 0; i < Particle.MAX; i++) {
				unowned Particle c = Particle.get(i);
				if (c.ticks_left == 0) continue;
				Vec2 p = S(c.p);
				if (c.type == ParticleType.HIT) {
					glPointSize((float)(0.3*c.ticks_left*view_scale/32));
					glColor4ub(255, 200, 200, c.ticks_left*8);
				} else if (c.type == ParticleType.PLASMA) {
					glPointSize((float)(0.15*c.ticks_left*view_scale/32));
					glColor4ub(255, 0, 0, c.ticks_left*32);
				} else if (c.type == ParticleType.ENGINE) {
					glPointSize((float)(0.1*c.ticks_left*view_scale/32));
					glColor4ub(255, 217, 43, 10 + c.ticks_left*5);
				} else if (c.type == ParticleType.EXPLOSION) {
					var s = c.v.abs();
					glPointSize((float)((0.05 + 0.05*c.ticks_left)*view_scale/32));
					GLubyte r = 255;
					GLubyte g = (GLubyte)(255*double.min(1.0, 0.0625*s+c.ticks_left*0.1));
					GLubyte b = 50;
					GLubyte a = 10 + c.ticks_left*20;
					glColor4ub(r, g, b, a);
				}
				glBegin(GL_POINTS);
				glVertex3d(p.x, p.y, 0);
				glEnd();
			}
		}

		private string fmt(double v, string unit) {
			var i = 0;
			var sign = v < 0 ? -1 : 1;
			var prefixes = " kMGTPEZY";
			for (i = 0; i < prefixes.length && sign*v >= 1000; i++) {
				v /= 1000;
			}
			if (sign*v < 1e-9) {
				v = 0;
			}
			var prefix = i == 0 ? "" : "%c".printf((int)prefixes[i]);
			return "%0.3g %s%s".printf(v, prefix, unit);
		}

		private void render_picked_info(Ship s) {
			int x = 15;
			int dy = 12;
			int y = 22+11*dy;
			var rv = s.physics.v.rotate(-s.physics.h);
			GLUtil.color32((uint32)0xAAFFFFAA);
			GLUtil.printf(x, y-0*dy, "%s %.8x %s", s.class.name, s.api_id, s.controlled ? "(player controlled)" : "");
			GLUtil.printf(x, y-1*dy, "hull: %s", fmt(s.hull,"J"));
			GLUtil.printf(x, y-2*dy, "position: (%s, %s)", fmt(s.physics.p.x,"m"), fmt(s.physics.p.y,"m"));
			GLUtil.printf(x, y-3*dy, "heading: %s", fmt(s.physics.h,"rad"));
			GLUtil.printf(x, y-4*dy, "velocity: (%s, %s) rel=(%s, %s)",
			                         fmt(s.physics.v.x,"m/s"), fmt(s.physics.v.y,"m/s"),
			                         fmt(rv.x,"m/s"), fmt(rv.y,"m/s"));
			GLUtil.printf(x, y-5*dy, "angular velocity: %s", fmt(s.physics.w,"rad/s"));
			GLUtil.printf(x, y-6*dy, "acceleration:");
			GLUtil.printf(x, y-7*dy, " main: %s", fmt(s.physics.acc.x,"m/s\xFD"));
			GLUtil.printf(x, y-8*dy, " lateral: %s", fmt(s.physics.acc.y,"m/s\xFD"));
			GLUtil.printf(x, y-9*dy, " angular: %s", fmt(s.physics.wa,"rad/s\xFD"));
			GLUtil.printf(x, y-10*dy, "energy: %s", fmt(s.get_energy(),"J"));
			GLUtil.printf(x, y-11*dy, "reaction mass: %s", fmt(s.reaction_mass*1000,"g"));
		}

		public void reshape(int width, int height) {
			screen_width = width;
			screen_height = height;
			glViewport (0, 0, (GLsizei)width, (GLsizei)height);
			glMatrixMode (GL_PROJECTION);
			glLoadIdentity ();
			glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
			glMatrixMode (GL_MODELVIEW);
			glLoadIdentity ();
		}

		public void tick() {
			foreach (unowned Bullet b in game.all_bullets) {
				if (b.dead) continue;
				if (b.type == BulletType.PLASMA) {
					Particle.shower(ParticleType.PLASMA, b.physics.p, vec2(0,0), b.physics.v.scale(1.0/63),
							            double.min(b.physics.m/5,0.1)*80, 3, 4, 6);
				} else if (b.type == BulletType.EXPLOSION) {
					if (prng.next_double() < 0.1) {
						Particle.shower(ParticleType.EXPLOSION, b.physics.p, vec2(0,0), b.physics.v.scale(Game.TICK_LENGTH).scale(0.001), 8, 5, 17, 6);
					}
				}
			}

			foreach (unowned BulletHit hit in game.bullet_hits) {
				var n = uint16.max((uint16)(hit.e/1000),1);
				Particle.shower(ParticleType.HIT, hit.cp, hit.s.physics.v.scale(Game.TICK_LENGTH), vec2(0,0), 8, 1, 20, n);
			}

			foreach (unowned Ship s in game.all_ships) {
				if (s.physics.acc.abs() != 0) {
					var vec_main = vec2(-s.physics.acc.x, 0).rotate(s.physics.h).scale(s.physics.m/1000);
					var vec_lateral = vec2(0, -s.physics.acc.y).rotate(s.physics.h).scale(s.physics.m/1000);
					Particle.shower(ParticleType.ENGINE, s.physics.p, s.physics.v.scale(Game.TICK_LENGTH), vec_main.scale(Game.TICK_LENGTH), 1, 2, 4, 8);
					Particle.shower(ParticleType.ENGINE, s.physics.p, s.physics.v.scale(Game.TICK_LENGTH), vec_lateral.scale(Game.TICK_LENGTH), 1, 2, 4, 8);
				}
			}
		}

		public Vec2 center() {
			return vec2(screen_width/2, screen_height/2);
		}

		public Vec2 S(Vec2 p) {
			return p.sub(view_pos).scale(view_scale).add(center());
		}

		public Vec2 W(Vec2 o) {
			return o.sub(center()).scale(1/view_scale).add(view_pos);
		}

		// XXX find ship with minimum distance, allow 5 px error
		public void pick(int x, int y) {
			Vec2 p = W(vec2(x, y));
			picked = null;
			double min_dist = 10/view_scale;
			foreach (unowned Ship s in game.all_ships) {
				var dist = s.physics.p.distance(p);
				if (!s.dead && ((dist < min_dist) || (picked == null && dist < s.physics.r))) {
					picked = s;
					if (dist < min_dist) min_dist = dist;
				}
			}
		}

		// XXX const
		double zoom_force = 0.1;
		double min_view_scale = 0.05;
		double max_view_scale = 6.0;

		public void zoom(int x, int y, double f) {
			if (view_scale != min_view_scale && view_scale != max_view_scale) {
				view_pos = view_pos.scale(1-zoom_force).add(W(vec2(x,y)).scale(zoom_force));
			}
			view_scale *= f;
			view_scale = double.min(double.max(view_scale, min_view_scale), max_view_scale);
		}

		static void on_ship_created(Ship s)
		{
			s.gfx.class = ShipGfxClass.lookup(s.class.name);
		}
	}

	namespace GLUtil {
		public void printf(int x, int y, string fmt, ...) {
			va_list ap = va_list();
			var str = fmt.vprintf(ap);
			write(x, y, str);
		}

		public void write(int x, int y, string str)
		{
			assert(font != null);
			if (GLEW.ARB_window_pos) {
				GLEW.glWindowPos2i(x, y);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
				unowned uint8 *data = str.data;

				for (int i = 0; data[i] != 0; i++) {
					glBitmap(8, 8, 4, 4, 9, 0, (GLubyte*)font + 8*data[i]);
				}
			}
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
