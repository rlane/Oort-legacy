using GL;
using GLEW;

public errordomain Oort.ShaderError {
	COMPILE_FAILED,
	LINK_FAILED
}

class Oort.Shader {
	public GLuint id;

	public Shader(GLenum type, string name, uint8[] src) throws ShaderError {
		id = glCreateShader(type);
		glShaderSource(id, 1, { src }, { (GLint)src.length });
		glCompileShader(id);
		glCheck();

		GLint shader_ok;
		glGetShaderiv(id, GL_COMPILE_STATUS, out shader_ok);
		if (shader_ok == 0) {
			GLint log_length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, out log_length);
			var log = new uint8[log_length];
			glGetShaderInfoLog(id, log_length, out log_length, log);
			throw new ShaderError.COMPILE_FAILED(@"Failed to compile shader %s:\n$((string)log)", name);
		}
	}

	public Shader.from_resource(GLenum type, string resource) throws ShaderError {
		this(type, resource, Resources.load(@"shaders/$(resource)"));
	}
}

class Oort.VertexShader : Oort.Shader {
	public VertexShader(string name, uint8[] src) throws ShaderError {
		base(GL_VERTEX_SHADER, name, src);
	}

	public VertexShader.from_resource(string resource) throws ShaderError {
		base.from_resource(GL_VERTEX_SHADER, resource);
	}
}

class Oort.FragmentShader : Oort.Shader {
	public FragmentShader(string name, uint8[] src) throws ShaderError {
		base(GL_FRAGMENT_SHADER, name, src);
	}

	public FragmentShader.from_resource(string resource) throws ShaderError {
		base.from_resource(GL_FRAGMENT_SHADER, resource);
	}
}

class Oort.ShaderProgram {
	public GLuint id;

	VertexShader vertex_shader;
	FragmentShader fragment_shader;

	public ShaderProgram(string name, VertexShader vs, FragmentShader fs) throws ShaderError {
		id = glCreateProgram();
		vertex_shader = vs;
		fragment_shader = fs;
		glAttachShader(id, vs.id);
		glAttachShader(id, fs.id);
		glLinkProgram(id);
		glCheck();

		GLint program_ok;
		glGetProgramiv(id, GL_LINK_STATUS, out program_ok);
		if (program_ok == 0) {
			GLsizei log_length;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, out log_length);
			var log = new uint8[log_length];
			glGetProgramInfoLog(id, log_length, &log_length, (string)log);
			throw new ShaderError.LINK_FAILED("Failed to link shader program %s:\n%s", name, log);
		}
	}

	public ShaderProgram.from_resources(string name) throws ShaderError {
		var vs = new VertexShader.from_resource(@"$(name).v.glsl");
		var fs = new FragmentShader.from_resource(@"$(name).f.glsl");
		this(name, vs, fs);
	}

	public void use() {
		glUseProgram(id);
	}

	public GLint uniform(string name) {
		var x = glGetUniformLocation(id, name);
		glCheck();
		if (x == -1) {
			GLib.error("bad uniform %s", name);
		}
		return x;
	}

	public GLint u(string name) {
		return uniform(name);
	}

	public GLint attribute(string name) {
		var x = glGetAttribLocation(id, name);
		glCheck();
		if (x == -1) {
			GLib.error("bad attribute %s", name);
		}
		return x;
	}

	public GLint a(string name) {
		return attribute(name);
	}
}
