using GL;
using GLEW;
using Vector;

class Oort.BoundaryBatch : Oort.RenderBatch {
	ShaderProgram prog;
	Model circle_model;

	public override void init() throws Error {
		prog = new ShaderProgram.from_resources("ship");
		circle_model = Model.load("circle");
	}

	public override void render() {
		var shape = circle_model.shapes[0];
		prog.use();
		glUniformMatrix4fv(prog.u("p_matrix"), 1, false, renderer.p_matrix.data);
		Mat4f mv_matrix;
		float scale = (float)game.scn.radius;
		Mat4f.load_scale(out mv_matrix, scale, scale, scale); 
		glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);
		glUniform4f(prog.u("color"), 0.2f, 0.2f, 0.2f, 0.39f);
		glBindBuffer(GL_ARRAY_BUFFER, shape.buffer);
		glVertexAttribPointer(prog.a("vertex"), 2, GL_DOUBLE, false, 0, (void*) 0);
		glEnableVertexAttribArray(prog.a("vertex"));
		glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) shape.vertices.length);
		glDisableVertexAttribArray(prog.a("vertex"));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUseProgram(0);
		glCheck();
	}
}
