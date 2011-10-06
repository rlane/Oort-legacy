using GL;
using Vector;

class Oort.ParticleBatch : Oort.RenderBatch {
	ShaderProgram prog;
	Texture particle_tex;

	public override void init() throws Error {
		prog = new ShaderProgram.from_resources("particle");
		particle_tex = new ParticleTexture();
	}

	private struct ParticleData {
		public Vec4f color;
		public Vec2f initial_position;
		public Vec2f velocity;
		public float initial_time;
		public float lifetime;
		public float size;
	}

	public override void render() {
		var current_time = (float)(game.ticks*Game.TICK_LENGTH);
		glBlendFunc(GL_ONE, GL_ONE);
		prog.use();
		glUniformMatrix4fv(prog.u("p_matrix"), 1, false, renderer.p_matrix.data);
		glUniform1f(prog.u("current_time"), current_time);
		glUniform1i(prog.u("tex"), 0);
		glCheck();

		ParticleData[] data = {};

		for (int i = 0; i < Particle.MAX; i++) {
			float size;
			Vec4f color;
			unowned Particle c = Particle.get(i);
			float death_time = c.initial_time + c.lifetime;
			var ticks_left = (int)((death_time-current_time)/Game.TICK_LENGTH);
			if (ticks_left == 0) continue;
			if (c.type == ParticleType.HIT) {
				size = (float)(0.3*ticks_left);
				color = vec4f(1.0f, 0.78f, 0.78f, ticks_left*0.03125f);
			} else if (c.type == ParticleType.PLASMA) {
				size = (float)(0.4*ticks_left);
				color = vec4f(1.0f, 0.1f, 0.1f, ticks_left*0.125f);
			} else if (c.type == ParticleType.ENGINE) {
				size = (float)(0.1*ticks_left);
				color = vec4f(1.0f, 0.8f, 0.17f, 0.039f + ticks_left*0.02f);
			} else if (c.type == ParticleType.EXPLOSION) {
				var s = c.v.abs();
				size = (float)(0.05 + 0.05*ticks_left);
				float g = (float) (255*double.min(1.0, 0.0625*s+ticks_left*0.1))/256;
				color = vec4f(1.0f, g, 0.2f, 0.04f + ticks_left*0.078f);
			} else {
				error("unknown particle");
			}

			data += ParticleData() {
				color = color,
				initial_position = c.p.to_vec2f(),
				velocity = c.v.to_vec2f(),
				initial_time = c.initial_time,
				lifetime = c.lifetime,
				size = size*(float)renderer.view_scale*10
			};
		}

		GLsizei stride = (GLsizei) sizeof(ParticleData);
		glVertexAttribPointer(prog.a("initial_time"), 1, GL_FLOAT, false, stride, &data[0].initial_time);
		glVertexAttribPointer(prog.a("lifetime"), 1, GL_FLOAT, false, stride, &data[0].lifetime);
		glVertexAttribPointer(prog.a("initial_position"), 2, GL_FLOAT, false, stride, &data[0].initial_position);
		glVertexAttribPointer(prog.a("velocity"), 2, GL_FLOAT, false, stride, &data[0].velocity);
		glVertexAttribPointer(prog.a("color"), 4, GL_FLOAT, false, stride, &data[0].color);
		glVertexAttribPointer(prog.a("size"), 1, GL_FLOAT, false, stride, &data[0].size);

		glEnableVertexAttribArray(prog.a("initial_time"));
		glEnableVertexAttribArray(prog.a("lifetime"));
		glEnableVertexAttribArray(prog.a("initial_position"));
		glEnableVertexAttribArray(prog.a("velocity"));
		glEnableVertexAttribArray(prog.a("color"));
		glEnableVertexAttribArray(prog.a("size"));
		particle_tex.bind();
		glDrawArrays(GL_POINTS, 0, (GLsizei) data.length);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableVertexAttribArray(prog.a("initial_time"));
		glDisableVertexAttribArray(prog.a("lifetime"));
		glDisableVertexAttribArray(prog.a("initial_position"));
		glDisableVertexAttribArray(prog.a("velocity"));
		glDisableVertexAttribArray(prog.a("color"));
		glDisableVertexAttribArray(prog.a("size"));

		glUseProgram(0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCheck();
	}
}
