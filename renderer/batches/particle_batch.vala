using GL;
using Vector;

class Oort.ParticleBatch : Oort.RenderBatch {
	ShaderProgram prog;
	Texture particle_tex;

	public override void init() throws Error {
		prog = new ShaderProgram.from_resources("particle");
		particle_tex = new ParticleTexture();
	}

	public override void render() {
		var current_time = (float)(game.ticks*Game.TICK_LENGTH);
		glBlendFunc(GL_ONE, GL_ONE);
		prog.use();
		glUniformMatrix4fv(prog.u("p_matrix"), 1, false, renderer.p_matrix.data);
		glUniform1f(prog.u("current_time"), current_time);
		glUniform1f(prog.u("view_scale"), (float)renderer.view_scale);
		glUniform1i(prog.u("tex"), 0);

		ParticleData[] data = {};

		for (int i = 0; i < Particle.MAX; i++) {
			unowned Particle c = Particle.get(i);
			data += ParticleData() {
				initial_position = c.p.to_vec2f(),
				velocity = c.v.to_vec2f(),
				initial_time = c.initial_time,
				lifetime = c.lifetime,
				type = (float) c.type
			};
		}

		GLsizei stride = (GLsizei) sizeof(ParticleData);
		glVertexAttribPointer(prog.a("initial_position"), 2, GL_FLOAT, false, stride, &data[0].initial_position);
		glVertexAttribPointer(prog.a("velocity"), 2, GL_FLOAT, false, stride, &data[0].velocity);
		glVertexAttribPointer(prog.a("initial_time"), 1, GL_FLOAT, false, stride, &data[0].initial_time);
		glVertexAttribPointer(prog.a("lifetime"), 1, GL_FLOAT, false, stride, &data[0].lifetime);
		glVertexAttribPointer(prog.a("type"), 1, GL_FLOAT, false, stride, &data[0].type);

		glEnableVertexAttribArray(prog.a("initial_position"));
		glEnableVertexAttribArray(prog.a("velocity"));
		glEnableVertexAttribArray(prog.a("initial_time"));
		glEnableVertexAttribArray(prog.a("lifetime"));
		glEnableVertexAttribArray(prog.a("type"));
		particle_tex.bind();
		glDrawArrays(GL_POINTS, 0, (GLsizei) data.length);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableVertexAttribArray(prog.a("initial_position"));
		glDisableVertexAttribArray(prog.a("velocity"));
		glDisableVertexAttribArray(prog.a("initial_time"));
		glDisableVertexAttribArray(prog.a("lifetime"));
		glDisableVertexAttribArray(prog.a("type"));

		glUseProgram(0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}
