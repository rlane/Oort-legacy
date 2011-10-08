using GL;
using Vector;
using Math;

class Oort.TailBatch : Oort.RenderBatch {
	ShaderProgram prog;

	public override void init() throws Error {
		prog = new ShaderProgram.from_resources("tail");
	}

	public override void render() {
		prog.use();
		glEnableVertexAttribArray(prog.a("vertex"));
		glEnableVertexAttribArray(prog.a("alpha"));
		glUniformMatrix4fv(prog.u("p_matrix"), 1, false, renderer.p_matrix.data);

		foreach (unowned Ship s in game.all_ships) {
			var alpha = s.class.radius < 5 ? 0.4f : 0.67f;
			var colorv = vec4f((float)(((s.team.color>>24)&0xFF)/255.0), (float)(((s.team.color>>16)&0xFF)/255.0), (float)(((s.team.color>>8)&0xFF)/255.0), alpha);
			var segments = new float[Ship.TAIL_SEGMENTS*2];
			var alphas = new float[Ship.TAIL_SEGMENTS];
			glUniform4f(prog.u("color"), colorv.x, colorv.y, colorv.z, colorv.w/3.0f);
			glVertexAttribPointer(prog.a("vertex"), 2, GL_FLOAT, false, 0, segments);
			glVertexAttribPointer(prog.a("alpha"), 1, GL_FLOAT, false, 0, alphas);

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
		}

		glDisableVertexAttribArray(prog.a("vertex"));
		glDisableVertexAttribArray(prog.a("alpha"));
		glUseProgram(0);
	}
}
