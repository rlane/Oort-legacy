using GL;
using GLEW;
using Vector;
using Math;

namespace Oort {
	class Renderer {
		public bool render_all_debug_lines = false;
		public int screen_width = 640;
		public int screen_height = 480;
		public double view_scale;
		public Vec2 view_pos;
		public unowned Ship picked = null;
		public Game game;
		public bool follow_picked = false;
		public double frame_msecs;

		Rand prng;
		RendererResources resources;
		Texture font_tex;
		ShaderProgram ship_program;
		ShaderProgram tail_program;
		ShaderProgram particle_program;
		ShaderProgram beam_program;
		ShaderProgram bullet_program;
		ShaderProgram text_program;
		Mat4f p_matrix;
		Model circle_model;

		public static void static_init() {
			if (GLEW.init()) {
				error("GLEW initialization failed");
			}
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
			glEnable(GL_PROGRAM_POINT_SIZE);
			glLineWidth(1.2f);

			resources.models.foreach( (k,v) => v.build() );

			try {
				load_shaders();
			} catch (ShaderError e) {
				GLib.error("loading shaders failed:\n%s", e.message);
			}

			load_font();
		}

		public void load_font() {
			var tex = new Texture();
			tex.bind();
			glCheck();
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glCheck();
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glCheck();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glCheck();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glCheck();
			var n = 256;
			var data = new uint8[64*n];
			for (int i = 0; i < n; i++) {
				for (int x = 0; x < 8; x++) {
					for (int y = 0; y < 8; y++) {
						uint8 row = font[8*i+y];
						bool on = ((row >> x) & 1) == 1;
						data[n*8*y + 8*i + x] = on ? 255 : 0;
					}
				}
			}
			glTexImage2D(GL_TEXTURE_2D, 0, 1, n*8, 8, 0, GL_RED, GL_UNSIGNED_BYTE, data);
			glCheck();
			glBindTexture(GL_TEXTURE_2D, 0);
			glCheck();
			font_tex = tex;
		}

		public void load_shaders() throws ShaderError{
			ship_program = new ShaderProgram.from_resources("ship");
			tail_program = new ShaderProgram.from_resources("tail");
			beam_program = new ShaderProgram.from_resources("beam");
			bullet_program = new ShaderProgram.from_resources("bullet");
			particle_program = new ShaderProgram.from_resources("particle");
			text_program = new ShaderProgram.from_resources("text");
		}

		public void render() {
			prng.set_seed(0); // XXX tick seed
			TimeVal start_time = TimeVal();

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
				render_debug_lines(s);
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
					render_picked_circle(picked);
					render_picked_acceleration(picked);
					render_picked_path(picked);
					render_picked_info(picked);
				}
			}

			glFinish();
			const int million = 1000*1000;
			TimeVal end_time = TimeVal();
			long usecs = (end_time.tv_sec-start_time.tv_sec)*million + (end_time.tv_usec - start_time.tv_usec);
			frame_msecs = usecs/1000.0;
		}

		void render_text(int x, int y, string text) {
			var prog = text_program;
			var pos = pixel2screen(vec2(x,y));
			var spacing = 9.0f;

			var chars = new float[text.length];
			var indices = new float[text.length];
			for (int i = 0; i < text.length; i++) {
				chars[i] = (float) text[i];
				indices[i] = (float) i;
			}

			glPointSize(8);
			prog.use();
			font_tex.bind();
			glUniform1i(prog.u("tex"), 0);
			glUniform1f(prog.u("dist"), 2.0f*spacing/screen_width);
			glUniform2f(prog.u("position"), (float)pos.x, (float)pos.y);
			glVertexAttribPointer(prog.a("character"), 1, GL_FLOAT, false, 0, chars);
			glVertexAttribPointer(prog.a("index"), 1, GL_FLOAT, false, 0, indices);
			glEnableVertexAttribArray(prog.a("character"));
			glEnableVertexAttribArray(prog.a("index"));
			glDrawArrays(GL_POINTS, 0, (GLsizei) text.length);
			glDisableVertexAttribArray(prog.a("character"));
			glDisableVertexAttribArray(prog.a("index"));
			glUseProgram(0);
			glCheck();
		}

		void render_ship(Ship s) {
			var prog = ship_program;
			var model = resources.models.lookup(s.class.name);
			prog.use();
			glBindBuffer(GL_ARRAY_BUFFER, model.id);
			glVertexAttribPointer(prog.a("vertex"), 2, GL_DOUBLE, false, 0, (void*) 0);
			glEnableVertexAttribArray(prog.a("vertex"));
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

			glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);
			glUniformMatrix4fv(prog.u("p_matrix"), 1, false, p_matrix.data);
			glUniform4f(prog.u("color"), colorv.x, colorv.y, colorv.z, colorv.w);
			glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) model.vertices.length);
			glDisableVertexAttribArray(prog.a("vertex"));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glUseProgram(0);
			glCheck();
		}

		void render_ship_tail(Ship s) {
			var prog = tail_program;
			prog.use();
			var model = resources.models.lookup(s.class.name);
			var colorv = vec4f((float)(((s.team.color>>24)&0xFF)/255.0), (float)(((s.team.color>>16)&0xFF)/255.0), (float)(((s.team.color>>8)&0xFF)/255.0), (float)model.alpha);
			var segments = new float[Ship.TAIL_SEGMENTS*2];
			var alphas = new float[Ship.TAIL_SEGMENTS];
			glUniform4f(prog.u("color"), colorv.x, colorv.y, colorv.z, colorv.w/3.0f);
			glUniformMatrix4fv(prog.u("p_matrix"), 1, false, p_matrix.data);
			glVertexAttribPointer(prog.a("vertex"), 2, GL_FLOAT, false, 0, segments);
			glVertexAttribPointer(prog.a("alpha"), 1, GL_FLOAT, false, 0, alphas);
			glEnableVertexAttribArray(prog.a("vertex"));
			glEnableVertexAttribArray(prog.a("alpha"));
			glCheck();

			segments[0] = (float) s.physics.p.x;
			segments[1] = (float) s.physics.p.y;
			alphas[0] = 1.0f;

			int i;
			for (i = 0; i < Ship.TAIL_SEGMENTS-1; i++) {
				int j = s.tail_head - i - 1;
				if (j < 0) j += Ship.TAIL_SEGMENTS;
				Vec2 v = s.tail[j];
				if (isnan(v.x) != 0)
					break;
				segments[2+i*2] = (float) v.x;
				segments[2+i*2+1] = (float) v.y;
				alphas[1+i] = 1.0f -((float)i)/Ship.TAIL_SEGMENTS;
			}

			glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) i);
			glDisableVertexAttribArray(prog.a("vertex"));
			glDisableVertexAttribArray(prog.a("alpha"));
			glUseProgram(0);
			glCheck();
		}

		void render_debug_lines(Ship s) {
			if (s != picked && !render_all_debug_lines) {
				return;
			}

			var prog = ship_program;
			prog.use();
			glCheck();
			Mat4f mv_matrix;
			Mat4f.load_identity(out mv_matrix);
			glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);
			glUniformMatrix4fv(prog.u("p_matrix"), 1, false, p_matrix.data);
			glUniform4f(prog.u("color"), 0.29f, 0.83f, 0.8f, 0.66f);
			var vertices = new float[s.debug.num_lines*4];
			for (int j = 0; j < s.debug.num_lines; j++) {
				var a = s.debug.lines[j].a;
				var b = s.debug.lines[j].b;
				vertices[4*j+0] = (float)a.x;
				vertices[4*j+1] = (float)a.y;
				vertices[4*j+2] = (float)b.x;
				vertices[4*j+3] = (float)b.y;
			}
			glVertexAttribPointer(prog.a("vertex"), 2, GL_FLOAT, false, 0, vertices);
			glEnableVertexAttribArray(prog.a("vertex"));
			glDrawArrays(GL_LINES, 0, (GLsizei) s.debug.num_lines*2);
			glDisableVertexAttribArray(prog.a("vertex"));
			glUseProgram(0);
			glCheck();
		}

		void render_picked_circle(Ship s) {
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
			glVertexAttribPointer(prog.a("vertex"), 2, GL_DOUBLE, false, 0, (void*) 0);
			glEnableVertexAttribArray(prog.a("vertex"));
			glUniform4f(prog.u("color"), 0.8f, 0.8f, 0.8f, 0.67f);
			glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);
			glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) circle_model.vertices.length);
			glDisableVertexAttribArray(prog.a("vertex"));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glUseProgram(0);
			glCheck();
		}

		void render_picked_acceleration(Ship s) {
			float vertices[4] = { 0, 0, (float)s.physics.acc.x, (float)s.physics.acc.y };
			var prog = ship_program;
			prog.use();
			Mat4f rotation_matrix;
			Mat4f translation_matrix;
			Mat4f mv_matrix;
			Mat4f.load_rotation(out rotation_matrix, (float)s.physics.h, 0, 0, 1);
			Mat4f.load_translation(out translation_matrix, (float)s.physics.p.x, (float)s.physics.p.y, 0);
			Mat4f.multiply(out mv_matrix, ref translation_matrix, ref rotation_matrix);
			glVertexAttribPointer(prog.a("vertex"), 2, GL_FLOAT, false, 0, vertices);
			glEnableVertexAttribArray(prog.a("vertex"));
			glUniform4f(prog.u("color"), 0.8f, 0.8f, 0.8f, 0.46f);
			glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);
			glDrawArrays(GL_LINE_LOOP, 0, 2);
			glDisableVertexAttribArray(prog.a("vertex"));
			glUseProgram(0);
			glCheck();
		}

		void render_picked_path(Ship s) {
			int n = (int) (1/Game.TICK_LENGTH);
			float[] vertices = new float[n*2];
			Physics q = s.physics.copy();
			for (int j = 0; j < n; j++) {
				vertices[j*2+0] = (float) q.p.x;
				vertices[j*2+1] = (float) q.p.y;
				q.tick_one();
			}

			var prog = ship_program;
			prog.use();
			Mat4f mv_matrix;
			Mat4f.load_identity(out mv_matrix);
			glVertexAttribPointer(prog.a("vertex"), 2, GL_FLOAT, false, 0, vertices);
			glEnableVertexAttribArray(prog.a("vertex"));
			glUniform4f(prog.u("color"), 0.29f, 0.83f, 0.8f, 0.66f);
			glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);
			glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) n);
			glDisableVertexAttribArray(prog.a("vertex"));
			glUseProgram(0);
			glCheck();
		}

		private void render_bullet(Bullet b) {
			if (b.dead) return;

			var prog = bullet_program;
			prog.use();
			glUniformMatrix4fv(prog.u("p_matrix"), 1, false, p_matrix.data);
			glCheck();

			if (b.type == Oort.BulletType.SLUG) {
				var dp = b.physics.v.scale(1.0/64);
				var offset = b.physics.v.scale(prng.next_double()/64);
				var p1 = b.physics.p.add(offset);
				var p2 = b.physics.p.add(offset).add(dp);

				Mat4f mv_matrix;
				Mat4f.load_identity(out mv_matrix);
				glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);
				glCheck();

				Vec2f vertices[2] = { p1.to_vec2f(), p2.to_vec2f() };

				Vec4f colors[2] = {
					vec4f(0.27f, 0.27f, 0.27f, 0.33f),
					vec4f(0.27f, 0.27f, 0.27f, 1.0f)
				};

				glVertexAttribPointer(prog.a("vertex"), 2, GL_FLOAT, false, 0, vertices);
				glVertexAttribPointer(prog.a("color"), 4, GL_FLOAT, false, 0, colors);
				glEnableVertexAttribArray(prog.a("vertex"));
				glEnableVertexAttribArray(prog.a("color"));
				glDrawArrays(GL_LINES, 0, 2);
				glCheck();
				glDisableVertexAttribArray(prog.a("vertex"));
				glDisableVertexAttribArray(prog.a("color"));
			} else if (b.type == Oort.BulletType.REFUEL) {
				Mat4f scale_matrix;
				Mat4f translation_matrix;
				Mat4f mv_matrix;
				float scale = (float)b.physics.r;
				var color = vec4f(0.47f, 0.47f, 0.47f, 0.66f);
				Mat4f.load_scale(out scale_matrix, scale, scale, scale); 
				Mat4f.load_translation(out translation_matrix, (float)b.physics.p.x, (float)b.physics.p.y, 0); 
				Mat4f.multiply(out mv_matrix, ref translation_matrix, ref scale_matrix);
				glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);
				glVertexAttrib4fv(prog.a("color"), color.data);
				glBindBuffer(GL_ARRAY_BUFFER, circle_model.id);
				glVertexAttribPointer(prog.a("vertex"), 2, GL_DOUBLE, false, 0, (void*) 0);
				glEnableVertexAttribArray(prog.a("vertex"));
				glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) circle_model.vertices.length);
				glDisableVertexAttribArray(prog.a("vertex"));
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
			float length = (float)b.length*1.1f;
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

			glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);
			glUniformMatrix4fv(prog.u("p_matrix"), 1, false, p_matrix.data);
			glUniform4f(prog.u("color"), color.x, color.y, color.z, 1);

			glVertexAttribPointer(prog.a("vertex"), 2, GL_FLOAT, false, 0, vertices);
			glVertexAttribPointer(prog.a("texcoord"), 2, GL_FLOAT, false, 0, texcoords);
			glEnableVertexAttribArray(prog.a("vertex"));
			glEnableVertexAttribArray(prog.a("texcoord"));
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glDisableVertexAttribArray(prog.a("vertex"));
			glDisableVertexAttribArray(prog.a("texcoord"));
		}

		private void render_particles() {
			var prog = particle_program;
			prog.use();
			glUniformMatrix4fv(prog.u("p_matrix"), 1, false, p_matrix.data);
			glUniform1f(prog.u("current_time"), 0);
			glCheck();

			Vec4f[] colors = {};
			float[] initial_times = {};
			float[] lifetimes = {};
			Vec2f[] initial_positions = {};
			Vec2f[] velocities = {};
			float[] sizes = {};

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

				colors += color;
				initial_times += 0;
				lifetimes += (float)(c.ticks_left*Game.TICK_LENGTH);
				initial_positions += c.p.to_vec2f();
				velocities += c.v.to_vec2f();
				sizes += size*(float)view_scale*10;
			}

			glVertexAttribPointer(prog.a("initial_time"), 1, GL_FLOAT, false, 0, initial_times);
			glVertexAttribPointer(prog.a("lifetime"), 1, GL_FLOAT, false, 0, lifetimes);
			glVertexAttribPointer(prog.a("initial_position"), 2, GL_FLOAT, false, 0, initial_positions);
			glVertexAttribPointer(prog.a("velocity"), 2, GL_FLOAT, false, 0, velocities);
			glVertexAttribPointer(prog.a("color"), 4, GL_FLOAT, false, 0, colors);
			glVertexAttribPointer(prog.a("size"), 1, GL_FLOAT, false, 0, sizes);

			glEnableVertexAttribArray(prog.a("initial_time"));
			glEnableVertexAttribArray(prog.a("lifetime"));
			glEnableVertexAttribArray(prog.a("initial_position"));
			glEnableVertexAttribArray(prog.a("velocity"));
			glEnableVertexAttribArray(prog.a("color"));
			glEnableVertexAttribArray(prog.a("size"));
			glDrawArrays(GL_POINTS, 0, initial_positions.length);
			glDisableVertexAttribArray(prog.a("initial_time"));
			glDisableVertexAttribArray(prog.a("lifetime"));
			glDisableVertexAttribArray(prog.a("initial_position"));
			glDisableVertexAttribArray(prog.a("velocity"));
			glDisableVertexAttribArray(prog.a("color"));
			glDisableVertexAttribArray(prog.a("size"));

			glUseProgram(0);
			glCheck();
		}

		private void render_boundary() {
			var prog = ship_program;
			prog.use();
			glUniformMatrix4fv(prog.u("p_matrix"), 1, false, p_matrix.data);
			Mat4f mv_matrix;
			float scale = (float)game.scn.radius;
			Mat4f.load_scale(out mv_matrix, scale, scale, scale); 
			glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);
			glUniform4f(prog.u("color"), 0.2f, 0.2f, 0.2f, 0.39f);
			glBindBuffer(GL_ARRAY_BUFFER, circle_model.id);
			glVertexAttribPointer(prog.a("vertex"), 2, GL_DOUBLE, false, 0, (void*) 0);
			glEnableVertexAttribArray(prog.a("vertex"));
			glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) circle_model.vertices.length);
			glDisableVertexAttribArray(prog.a("vertex"));
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
			int y = screen_height - 22 - 11*dy;
			var rv = s.physics.v.rotate(-s.physics.h);
			textf(x, y+0*dy, "%s %s %s", s.class.name, s.hex_id, s.controlled ? "(player controlled)" : "");
			textf(x, y+1*dy, "hull: %s", fmt(s.hull,"J"));
			textf(x, y+2*dy, "position: (%s, %s)", fmt(s.physics.p.x,"m"), fmt(s.physics.p.y,"m"));
			textf(x, y+3*dy, "heading: %s", fmt(s.physics.h,"rad"));
			textf(x, y+4*dy, "velocity: (%s, %s) rel=(%s, %s)",
			                 fmt(s.physics.v.x,"m/s"), fmt(s.physics.v.y,"m/s"),
			                 fmt(rv.x,"m/s"), fmt(rv.y,"m/s"));
			textf(x, y+5*dy, "angular velocity: %s", fmt(s.physics.w,"rad/s"));
			textf(x, y+6*dy, "acceleration:");
			textf(x, y+7*dy, " main: %s", fmt(s.physics.acc.x,"m/s\xFD"));
			textf(x, y+8*dy, " lateral: %s", fmt(s.physics.acc.y,"m/s\xFD"));
			textf(x, y+9*dy, " angular: %s", fmt(s.physics.wa,"rad/s\xFD"));
			textf(x, y+10*dy, "energy: %s", fmt(s.get_energy(),"J"));
			textf(x, y+11*dy, "reaction mass: %s", fmt(s.get_reaction_mass()*1000,"g"));
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
		}

		public void textf(int x, int y, string fmt, ...) {
			va_list ap = va_list();
			var str = fmt.vprintf(ap);
			render_text(x, y, str);
		}
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
