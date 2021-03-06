using GL;
using Vector;

class Oort.BulletBatch : Oort.RenderBatch {
	ShaderProgram prog;
	Model circle_model;

	public override void init() throws Error {
		prog = new ShaderProgram.from_resources("bullet");
		circle_model = Model.load("circle");
	}

	public override void render() {
		prog.use();
		glEnableVertexAttribArray(prog.a("vertex"));
		glEnableVertexAttribArray(prog.a("color"));
		glUniformMatrix4fv(prog.u("p_matrix"), 1, false, renderer.p_matrix.data);

		Bullet*[] slugs = {};
		Bullet*[] refuels = {};

		foreach (unowned Bullet b in game.all_bullets) {
			if (b.dead) continue;
			if (b.type == Oort.BulletType.SLUG) {
				slugs += b;
			} else if (b.type == Oort.BulletType.REFUEL) {
				refuels += b;
			}
		}

		render_slugs(slugs);
		render_refuels(refuels);

		glDisableVertexAttribArray(prog.a("color"));
		glDisableVertexAttribArray(prog.a("vertex"));
		glUseProgram(0);
	}

	void render_slugs(Bullet[] bullets) {
		Vec4f colors[2] = {
			vec4f(0.27f, 0.27f, 0.27f, 0.33f),
			vec4f(0.27f, 0.27f, 0.27f, 1.0f)
		};

		glVertexAttribPointer(prog.a("color"), 4, GL_FLOAT, false, 0, colors);

		Mat4f mv_matrix;
		Mat4f.load_identity(out mv_matrix);
		glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);

		foreach (unowned Bullet b in bullets) {
			var dp = b.physics.v.scale(1.0/64);
			var offset = b.physics.v.scale(renderer.prng.next_double()/64);
			var p1 = b.physics.p.add(offset);
			var p2 = b.physics.p.add(offset).add(dp);

			Vec2f vertices[2] = { p1.to_vec2f(), p2.to_vec2f() };
			glVertexAttribPointer(prog.a("vertex"), 2, GL_FLOAT, false, 0, vertices);
			glDrawArrays(GL_LINES, 0, 2);
		}
	}

	void render_refuels(Bullet[] bullets) {
		Mat4f scale_matrix;
		Mat4f translation_matrix;
		Mat4f mv_matrix;
		var shape = circle_model.shapes[0];
		var color = vec4f(0.47f, 0.47f, 0.47f, 0.66f);
		glVertexAttrib4fv(prog.a("color"), color.data);
		glEnableVertexAttribArray(prog.a("vertex"));
		glBindBuffer(GL_ARRAY_BUFFER, shape.buffer);
		glVertexAttribPointer(prog.a("vertex"), 2, GL_FLOAT, false, 0, (void*) 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		foreach (unowned Bullet b in bullets) {
			float scale = (float)b.physics.r;
			Mat4f.load_scale(out scale_matrix, scale, scale, scale); 
			Mat4f.load_translation(out translation_matrix, (float)b.physics.p.x, (float)b.physics.p.y, 0); 
			Mat4f.multiply(out mv_matrix, ref translation_matrix, ref scale_matrix);
			glUniformMatrix4fv(prog.u("mv_matrix"), 1, false, mv_matrix.data);
			glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) shape.vertices.length);
		}

		glDisableVertexAttribArray(prog.a("vertex"));
	}
}
