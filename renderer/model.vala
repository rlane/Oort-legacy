using GL;
using GLEW;
using Oort;
using Vector;

public errordomain Oort.ModelParseError {
	JSON_SYNTAX,
	WRONG_TYPE,
}

[SimpleType]
public class Oort.Shape {
	public GLuint buffer;
	public Vec2f[] vertices;

	public Shape(Vec2f[] vs) {
		vertices = vs;
		buffer = 0;
	}
}

public class Oort.Model {
	public Shape[] shapes;
	public double alpha;

	public static Model load(string name) {
		var data = Resources.load(@"models/$name.json");
		try {
			var model = new Model(data);
			model.build();
			return model;
		} catch (ModelParseError e) {
			GLib.error("%s when parsing model %s", e.message, name);
		}
	}

	public Model(uint8[] data) throws ModelParseError {
		Shape[] tmp_shapes = {};

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
		alpha = alpha_node.double;

		unowned cJSON shapes_node = root.objectItem("shapes");
		if (shapes_node == null || shapes_node.type != cJSON.Type.Array) {
			throw new ModelParseError.WRONG_TYPE("shapes field must be an array");
		}

		unowned cJSON shape_node = shapes_node.child;
		while (shape_node != null) {
			Vec2f[] tmp_vertices = {};

			if (shape_node.type != cJSON.Type.Array) {
				throw new ModelParseError.WRONG_TYPE("shape field must be an array");
			}

			unowned cJSON vertex_node = shape_node.child;
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

				tmp_vertices += vec2f((float)x_node.double, (float)y_node.double);

				vertex_node = vertex_node.next;
			}
		
			tmp_shapes += new Shape((owned) tmp_vertices);

			shape_node = shape_node.next;
		}

		shapes = (owned) tmp_shapes;
	}

	public void build() {
		var n = shapes.length;
		for (int i = 0; i < n; i++) {
			var shape = shapes[i];
			var vertices = shape.vertices;
			glGenBuffers(1, &shape.buffer);
			glBindBuffer(GL_ARRAY_BUFFER, shape.buffer);
			glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (vertices.length*sizeof(Vec2f)), (void*) (&vertices[0]), GL_STATIC_DRAW);
			glCheck();
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
