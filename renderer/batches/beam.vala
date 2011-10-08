using GL;
using Vector;

class Oort.BeamBatch : Oort.RenderBatch {
	ShaderProgram prog;

	public override void init() throws Error {
		prog = new ShaderProgram.from_resources("beam");
	}

	public override void render() {
		prog.use();
		glUniformMatrix4fv(prog.u("p_matrix"), 1, false, renderer.p_matrix.data);
		glEnableVertexAttribArray(prog.a("vertex"));
		glEnableVertexAttribArray(prog.a("texcoord"));

		float texcoords[8] = {
			0, 1,
			0, 0,
			1, 1,
			1, 0
		};

		glVertexAttribPointer(prog.a("texcoord"), 2, GL_FLOAT, false, 0, texcoords);

		foreach (unowned Beam b in game.all_beams) {
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

			glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);
			glUniform4f(prog.u("color"), color.x, color.y, color.z, 1);

			glVertexAttribPointer(prog.a("vertex"), 2, GL_FLOAT, false, 0, vertices);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		glDisableVertexAttribArray(prog.a("vertex"));
		glDisableVertexAttribArray(prog.a("texcoord"));
		glUseProgram(0);
	}
}
