using GL;
using GLEW;
using Oort;
using Vector;

public errordomain Oort.ModelParseError {
	JSON_SYNTAX,
	WRONG_TYPE,
}

public class Oort.Model {
	public GLuint id;
	public Vec2[] vertices;
	public uint8 alpha;

	public Model(uint8[] data) throws ModelParseError {
		Vec2[] tmp_vertices = {};

		var root = cJSON.parse((string)data);
		if (root == null) {
			throw new ModelParseError.JSON_SYNTAX("JSON syntax incorrect");
		} else if (root.type != cJSON.Type.Object) {
			throw new ModelParseError.WRONG_TYPE("root must be an object");
		}

		unowned cJSON alpha_node = root.objectItem("alpha");
		if (alpha_node == null || alpha_node.type != cJSON.Type.Number) {
			throw new ModelParseError.WRONG_TYPE("alpha field must be a number");
		}
		alpha = (uint8) alpha_node.int;

		unowned cJSON vertices_node = root.objectItem("vertices");
		if (vertices_node == null || vertices_node.type != cJSON.Type.Array) {
			throw new ModelParseError.WRONG_TYPE("vertices field must be an array");
		}

		unowned cJSON vertex_node = vertices_node.child;
		while (vertex_node != null) {
			if (vertex_node.type != cJSON.Type.Object) {
				throw new ModelParseError.WRONG_TYPE("vertex must be an object");
			}

			unowned cJSON x_node = vertex_node.objectItem("x");
			if (x_node == null || x_node.type != cJSON.Type.Number) {
				throw new ModelParseError.WRONG_TYPE("field vertex.x must be a number");
			}

			unowned cJSON y_node = vertex_node.objectItem("y");
			if (y_node == null || y_node.type != cJSON.Type.Number) {
				throw new ModelParseError.WRONG_TYPE("field vertex.y must be a number");
			}

			tmp_vertices += vec2(x_node.double, y_node.double);

			vertex_node = vertex_node.next;
		}

		vertices = (owned) tmp_vertices;
	}

	public void build() {
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (vertices.length*sizeof(Vec2)), (void*) (&vertices[0]), GL_STATIC_DRAW);
		glCheck();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	public void render() {
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) vertices.length);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glCheck();
	}
}
