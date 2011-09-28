using GL;
using GLEW;
using Vector;

class Oort.ShipBatch : Oort.RenderBatch {
	ShaderProgram prog;
	public HashTable<string,Model> models = new HashTable<string,Model>(str_hash, str_equal);

	public override void init() throws Error {
		prog = new ShaderProgram.from_resources("ship");

		ShipClass.ship_classes.foreach((k,v) => {
			models.insert(k, Model.load(k));
		});
	}

	public override void render() {
		foreach (unowned Ship s in game.all_ships) {
			var model = models.lookup(s.class.name);
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

			var colorv = vec4f((float)(((s.team.color>>24)&0xFF)/255.0), (float)(((s.team.color>>16)&0xFF)/255.0), (float)(((s.team.color>>8)&0xFF)/255.0), (float)model.alpha);

			glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);
			glUniformMatrix4fv(prog.u("p_matrix"), 1, false, renderer.p_matrix.data);
			glUniform4f(prog.u("color"), colorv.x, colorv.y, colorv.z, colorv.w);
			glEnableVertexAttribArray(prog.a("vertex"));

			foreach (var shape in model.shapes) {
				glBindBuffer(GL_ARRAY_BUFFER, shape.buffer);
				glVertexAttribPointer(prog.a("vertex"), 2, GL_DOUBLE, false, 0, (void*) 0);
				glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) shape.vertices.length);
			}

			glDisableVertexAttribArray(prog.a("vertex"));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glUseProgram(0);
			glCheck();
		}
	}
}
