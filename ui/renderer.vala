using GL;
using GLEW;
using Vector;
using Math;

[Compact]
public class Oort.ShipGfxClass {
	public static ShipGfxClass fighter;
	public static ShipGfxClass ion_cannon_frigate;
	public static ShipGfxClass assault_frigate;
	public static ShipGfxClass carrier;
	public static ShipGfxClass missile;
	public static ShipGfxClass torpedo;
	public static ShipGfxClass unknown;
	
	public static void init() {
		fighter = new ShipGfxClass();
		ion_cannon_frigate = new ShipGfxClass();
		assault_frigate = new ShipGfxClass();
		carrier = new ShipGfxClass();
		missile = new ShipGfxClass();
		torpedo = new ShipGfxClass();
		unknown = new ShipGfxClass();
	}

	public static unowned ShipGfxClass lookup(string name)
	{
		switch (name) {
			case "fighter": return fighter;
			case "ion_cannon_frigate": return ion_cannon_frigate;
			case "assault_frigate": return assault_frigate;
			case "carrier": return carrier;
			case "missile": return missile;
			case "torpedo": return torpedo;
			default: return unknown;
		}
	}
}

namespace Oort {
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
		RendererResources resources;
		Texture ion_beam_tex;
		Texture laser_beam_tex;
		ShipProgram ship_program;
		ParticleProgram particle_program;
		BeamProgram beam_program;
		Mat4f p_matrix;
		Model circle_model;

		public static void static_init() {
			if (GLEW.init()) {
				error("GLEW initialization failed");
			}
			ShipGfxClass.init();
			Oort.Ship.gfx_create_cb = on_ship_created;

			/*
			print("Vendor: %s\n", glGetString(GL_VENDOR));
			print("Renderer: %s\n", glGetString(GL_RENDERER));
			print("GL Version: %s\n", glGetString(GL_VERSION));
			//print("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
			print("Extensions:\n%s\n", glGetString(GL_EXTENSIONS));
			*/
		}

		public Renderer(Game game,
		                RendererResources resources,
		                double initial_view_scale) {
			this.game = game;
			this.resources = resources;
			view_scale = initial_view_scale;
			prng = new Rand();
			view_pos = vec2(0,0);

			circle_model = resources.models.lookup("circle");
		}

		public void init() {
			glClearColor(0.0f, 0.0f, 0.03f, 0.0f);
			glShadeModel(GL_SMOOTH);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SPRITE);
			glLineWidth(1.2f);

			resources.models.foreach( (k,v) => v.build() );

			try {
				load_shaders();
			} catch (ShaderError e) {
				GLib.error("loading shaders failed:\n%s", e.message);
			}
		}

		public void load_shaders() throws ShaderError{
			ship_program = new ShipProgram();
			particle_program = new ParticleProgram();
			beam_program = new BeamProgram();
		}

		public void render() {
			prng.set_seed(0); // XXX tick seed

			glEnable(GL_BLEND);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();

			Mat4f.load_simple_ortho(out p_matrix,
			                        (float)this.view_pos.x,
			                        (float)this.view_pos.y,
			                        (float)screen_height/(float)screen_width,
			                        (float)(2000.0/view_scale));

			glBlendFunc(GL_ONE, GL_ONE);

			render_particles();

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			render_boundary();

			if (follow_picked && picked != null) {
				view_pos = picked.physics.p;
			}

			foreach (unowned Ship s in game.all_ships) {
				render_ship(s);
				render_ship_tail(s);
			}

			foreach (unowned Bullet b in game.all_bullets) {
				render_bullet(b);
			}

			foreach (unowned Beam b in game.all_beams) {
				render_beam(b);
			}
			
			if (picked != null) {
				if (picked.dead) {
					picked = null;
				} else {
					render_picked_stuff(picked);
					render_picked_info(picked);
				}
			}
		}

		void render_ship(Ship s) {
			var prog = ship_program;
			var model = resources.models.lookup(s.class.name);
			prog.use();
			glBindBuffer(GL_ARRAY_BUFFER, model.id);
			glVertexAttribPointer(prog.a_vertex, 2, GL_DOUBLE, false, 0, (void*) 0);
			glEnableVertexAttribArray(prog.a_vertex);
			glCheck();

			Mat4f rotation_matrix;
			Mat4f translation_matrix;
			Mat4f scale_matrix;
			Mat4f mv_matrix;
			Mat4f tmp_matrix;

			Mat4f.load_rotation(out rotation_matrix, (float)s.physics.h, 0, 0, 1);
			Mat4f.load_translation(out translation_matrix, (float)s.physics.p.x, (float)s.physics.p.y, 0);
			Mat4f.load_scale(out scale_matrix, (float)s.class.radius, (float)s.class.radius, (float)s.class.radius);
			Mat4f.multiply(out tmp_matrix, ref rotation_matrix, ref scale_matrix);
			Mat4f.multiply(out mv_matrix, ref translation_matrix, ref tmp_matrix);

			var colorv = vec4f((float)(((s.team.color>>24)&0xFF)/255.0), (float)(((s.team.color>>16)&0xFF)/255.0), (float)(((s.team.color>>8)&0xFF)/255.0), (float)model.alpha);

			glUniformMatrix4fv(prog.u_mv_matrix, 1, false, mv_matrix.data);
			glUniformMatrix4fv(prog.u_p_matrix, 1, false, p_matrix.data);
			glUniform4f(prog.u_color, colorv.x, colorv.y, colorv.z, colorv.w);
			glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) model.vertices.length);
			glDisableVertexAttribArray(prog.a_vertex);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glUseProgram(0);
			glCheck();
		}

		void render_ship_tail(Ship s) {
			var prog = ship_program;
			prog.use();
			var model = resources.models.lookup(s.class.name);
			var colorv = vec4f((float)(((s.team.color>>24)&0xFF)/255.0), (float)(((s.team.color>>16)&0xFF)/255.0), (float)(((s.team.color>>8)&0xFF)/255.0), (float)model.alpha);
			float tail_alpha = colorv.w/3.0f;
			var segments = new float[Ship.TAIL_SEGMENTS*2];
			glUniform4f(prog.u_color, colorv.x, colorv.y, colorv.z, tail_alpha);
			glVertexAttribPointer(prog.a_vertex, 2, GL_FLOAT, false, 0, segments);
			glEnableVertexAttribArray(prog.a_vertex);
			Mat4f identity_matrix;
			Mat4f.load_identity(out identity_matrix);
			glUniformMatrix4fv(prog.u_mv_matrix, 1, false, identity_matrix.data);
			glCheck();

			segments[0] = (float) s.physics.p.x;
			segments[1] = (float) s.physics.p.y;
			int i;
			for (i = 0; i < Ship.TAIL_SEGMENTS-1; i++) {
				int j = s.tail_head - i - 1;
				if (j < 0) j += Ship.TAIL_SEGMENTS;
				Vec2 v = s.tail[j];
				if (isnan(v.x) != 0)
					break;
				//uint32 color = s.team.color | (tail_alpha_max-(tail_alpha_max/Ship.TAIL_SEGMENTS)*i);
				segments[2+i*2] = (float) v.x;
				segments[2+i*2+1] = (float) v.y;
			}

			glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) i);
			glDisableVertexAttribArray(prog.a_vertex);
			glUseProgram(0);
			glCheck();
		}

		void render_debug_lines(Ship s) {
/*
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
*/
		}

		void render_picked_stuff(Ship s) {
			var prog = ship_program;
			prog.use();
			Mat4f rotation_matrix;
			Mat4f translation_matrix;
			Mat4f scale_matrix;
			Mat4f mv_matrix;
			Mat4f tmp_matrix;
			Mat4f.load_rotation(out rotation_matrix, (float)s.physics.h, 0, 0, 1);
			Mat4f.load_translation(out translation_matrix, (float)s.physics.p.x, (float)s.physics.p.y, 0);
			Mat4f.load_scale(out scale_matrix, (float)s.class.radius, (float)s.class.radius, (float)s.class.radius);
			Mat4f.multiply(out tmp_matrix, ref rotation_matrix, ref scale_matrix);
			Mat4f.multiply(out mv_matrix, ref translation_matrix, ref tmp_matrix);
			glBindBuffer(GL_ARRAY_BUFFER, circle_model.id);
			glVertexAttribPointer(prog.a_vertex, 2, GL_DOUBLE, false, 0, (void*) 0);
			glEnableVertexAttribArray(prog.a_vertex);
			glUniform4f(prog.u_color, 0.8f, 0.8f, 0.8f, 0.67f);
			glUniformMatrix4fv(prog.u_mv_matrix, 1, false, mv_matrix.data);
			glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) circle_model.vertices.length);
			glDisableVertexAttribArray(prog.a_vertex);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glUseProgram(0);
			glCheck();

/*
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
*/
		}

		private void render_bullet(Bullet b) {
			if (b.dead) return;

			var prog = ship_program;
			prog.use();
			glUniformMatrix4fv(prog.u_p_matrix, 1, false, p_matrix.data);
			glCheck();

			if (b.type == Oort.BulletType.SLUG) {
				var dp = b.physics.v.scale(1.0/64);
				var offset = b.physics.v.scale(prng.next_double()/64);
				var p1 = b.physics.p.add(offset);
				var p2 = b.physics.p.add(offset).add(dp);

				Mat4f mv_matrix;
				Mat4f.load_identity(out mv_matrix);
				glUniformMatrix4fv(prog.u_mv_matrix, 1, false, mv_matrix.data);
				glUniform4f(prog.u_color, 0.27f, 0.27f, 0.27f, 1.0f); // XXX fade to alpha 0.33
				glCheck();

				//Oort.GLUtil.color32(0x44444455);
				//Oort.GLUtil.color32(0x444444FF);

				float line[4] = {
					(float) p1.x, (float) p1.y,
					(float) p2.x, (float) p2.y
				};

				glVertexAttribPointer(prog.a_vertex, 2, GL_FLOAT, false, 0, line);
				glEnableVertexAttribArray(prog.a_vertex);
				glDrawArrays(GL_LINES, 0, 2);
				glCheck();

				glDisableVertexAttribArray(prog.a_vertex);
			} else if (b.type == Oort.BulletType.REFUEL) {
				Mat4f scale_matrix;
				Mat4f translation_matrix;
				Mat4f mv_matrix;
				float scale = (float)b.physics.r;
				Mat4f.load_scale(out scale_matrix, scale, scale, scale); 
				Mat4f.load_translation(out translation_matrix, (float)b.physics.p.x, (float)b.physics.p.y, 0); 
				Mat4f.multiply(out mv_matrix, ref translation_matrix, ref scale_matrix);
				glUniformMatrix4fv(prog.u_mv_matrix, 1, false, mv_matrix.data);
				glUniform4f(prog.u_color, 0.47f, 0.47f, 0.47f, 0.66f);
				glBindBuffer(GL_ARRAY_BUFFER, circle_model.id);
				glVertexAttribPointer(prog.a_vertex, 2, GL_DOUBLE, false, 0, (void*) 0);
				glEnableVertexAttribArray(prog.a_vertex);
				glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) circle_model.vertices.length);
				glDisableVertexAttribArray(prog.a_vertex);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glCheck();
			}

			glUseProgram(0);
			glCheck();
		}

		private void render_beam(Beam b) {
			var prog = beam_program;
			prog.use();

			Vec4f color;
			float offset = 0.0f;
			float length = (float)b.length;
			float width = (float)b.width/2.0f;

			if (b.graphics == Oort.BeamGraphics.ION) {
				color = vec4f(0.5f, 0.5f, 1.0f, 0);
				offset = 0.7f*40;
			} else if (b.graphics == Oort.BeamGraphics.LASER) {
				color = vec4f(1.0f, 0.1f, 0.1f, 0);
			} else {
				error("unknown beam");
			}

			Mat4f rotation_matrix;
			Mat4f translation_matrix;
			Mat4f scale_matrix;
			Mat4f mv_matrix;
			Mat4f tmp_matrix;
			Mat4f.load_rotation(out rotation_matrix, (float)b.a, 0, 0, 1);
			Mat4f.load_translation(out translation_matrix, (float)b.p.x, (float)b.p.y, 0);
			Mat4f.load_identity(out scale_matrix);
			Mat4f.multiply(out mv_matrix, ref translation_matrix, ref rotation_matrix);

			float vertices[8] = {
				offset, width,
				offset, -width,
				length, width,
				length, -width
			};

			float texcoords[8] = {
				0, 1,
				0, 0,
				1, 1,
				1, 0
			};

			glUniformMatrix4fv(prog.u_mv_matrix, 1, false, mv_matrix.data);
			glUniformMatrix4fv(prog.u_p_matrix, 1, false, p_matrix.data);
			glUniform4f(prog.u_color, color.x, color.y, color.z, 1);

			glVertexAttribPointer(prog.a_vertex, 2, GL_FLOAT, false, 0, vertices);
			glVertexAttribPointer(prog.a_texcoord, 2, GL_FLOAT, false, 0, texcoords);
			glEnableVertexAttribArray(prog.a_vertex);
			glEnableVertexAttribArray(prog.a_texcoord);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glDisableVertexAttribArray(prog.a_vertex);
			glDisableVertexAttribArray(prog.a_texcoord);
		}

		private void render_particles() {
			var prog = particle_program;
			prog.use();
			glUniformMatrix4fv(prog.u_p_matrix, 1, false, p_matrix.data);
			glCheck();

			for (int i = 0; i < Particle.MAX; i++) {
				float size;
				Vec4f color;
				unowned Particle c = Particle.get(i);
				if (c.ticks_left == 0) continue;
				if (c.type == ParticleType.HIT) {
					size = (float)(0.3*c.ticks_left);
					color = vec4f(1.0f, 0.78f, 0.78f, c.ticks_left*0.03125f);
				} else if (c.type == ParticleType.PLASMA) {
					size = (float)(0.4*c.ticks_left);
					color = vec4f(1.0f, 0.1f, 0.1f, c.ticks_left*0.125f);
				} else if (c.type == ParticleType.ENGINE) {
					size = (float)(0.1*c.ticks_left);
					color = vec4f(1.0f, 0.8f, 0.17f, 0.039f + c.ticks_left*0.02f);
				} else if (c.type == ParticleType.EXPLOSION) {
					var s = c.v.abs();
					size = (float)(0.05 + 0.05*c.ticks_left);
					float g = (float) (255*double.min(1.0, 0.0625*s+c.ticks_left*0.1))/256;
					color = vec4f(1.0f, g, 0.2f, 0.04f + c.ticks_left*0.078f);
				} else {
					error("unknown particle");
				}

				glPointSize(size*(float)view_scale*10);
				float position[2] = { (float)c.p.x, (float)c.p.y };

				glUniform4f(prog.u_color, color.x, color.y, color.z, color.w);
				glVertexAttribPointer(prog.a_position, 2, GL_FLOAT, false, 0, position);
				glEnableVertexAttribArray(prog.a_position);
				glDrawArrays(GL_POINTS, 0, 1);
				glDisableVertexAttribArray(prog.a_position);
				glCheck();
			}

			glUseProgram(0);
			glCheck();
		}

		private void render_boundary() {
			var prog = ship_program;
			prog.use();
			glUniformMatrix4fv(prog.u_p_matrix, 1, false, p_matrix.data);
			Mat4f mv_matrix;
			float scale = (float)game.scn.radius;
			Mat4f.load_scale(out mv_matrix, scale, scale, scale); 
			glUniformMatrix4fv(prog.u_mv_matrix, 1, false, mv_matrix.data);
			glUniform4f(prog.u_color, 0.2f, 0.2f, 0.2f, 0.39f);
			glBindBuffer(GL_ARRAY_BUFFER, circle_model.id);
			glVertexAttribPointer(prog.a_vertex, 2, GL_DOUBLE, false, 0, (void*) 0);
			glEnableVertexAttribArray(prog.a_vertex);
			glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) circle_model.vertices.length);
			glDisableVertexAttribArray(prog.a_vertex);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glUseProgram(0);
			glCheck();
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
			GLUtil.printf(x, y-0*dy, "%s %s %s", s.class.name, s.hex_id, s.controlled ? "(player controlled)" : "");
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
			GLUtil.printf(x, y-11*dy, "reaction mass: %s", fmt(s.get_reaction_mass()*1000,"g"));
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
					                double.min(b.physics.m/5,0.1)*80, 3, 4, 9);
				} else if (b.type == BulletType.EXPLOSION) {
					if (prng.next_double() < 0.1) {
						Particle.shower(ParticleType.EXPLOSION, b.physics.p, vec2(0,0), b.physics.v.scale(Game.TICK_LENGTH).scale(0.001), 8, 5, 17, 6);
					}
				}
			}

			foreach (unowned BulletHit hit in game.bullet_hits) {
				var n = uint16.max((uint16)(hit.e/10000),1);
				Particle.shower(ParticleType.HIT, hit.cp, hit.s.physics.v.scale(Game.TICK_LENGTH), vec2(0,0), 8, 1, 20, n);
			}

			foreach (unowned BeamHit hit in game.beam_hits) {
				var n = uint16.max((uint16)(hit.e/500),1);
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

		public Vec2 pixel2screen(Vec2 p) {
			return vec2((float) (2*p.x/screen_width-1),
			            (float) (-2*p.y/screen_height+1));
		}

		public Vec2 W(Vec2 o) {
			Mat4f m;
			Vec2 screen_coord = pixel2screen(o);
			Vec4f v = vec4f((float)screen_coord.x, (float)screen_coord.y, 0, 0);
			Mat4f.invert(out m, ref p_matrix);
			var v2 = v.transform(ref m);
			v2.x += (float)view_pos.x;
			v2.y += (float)view_pos.y;
			return vec2(v2.x, v2.y);
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

public class Oort.RendererResources {
	public HashTable<string,Model> models = new HashTable<string,Model>(str_hash, str_equal);

	public RendererResources() throws ModelParseError, FileError, Error {
		var directory = File.new_for_path(data_path("models"));
		var enumerator = directory.enumerate_children (FILE_ATTRIBUTE_STANDARD_NAME, 0);

		FileInfo file_info;
		while ((file_info = enumerator.next_file ()) != null) {
			var filename = file_info.get_name();
			if (!filename.has_suffix(".json")) {
				continue;
			}
			var name = filename[0:-5];

			var data = Game.load_resource(@"models/$filename");
			try {
				models.insert(name, new Model(data));
			} catch (ModelParseError e) {
				e.message += @" when parsing $name";
				throw e;
			}
		}
	}

	public void build() {
	}
}
