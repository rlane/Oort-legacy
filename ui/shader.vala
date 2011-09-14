using GL;
using GLEW;

class Oort.Shader {
	public GLuint id;

	public Shader(GLenum type, uint8[] src) {
		id = glCreateShader(type);
		glCheck();
		glShaderSource(id, 1, { src }, { (GLint)src.length });
		glCheck();
		glCompileShader(id);
		glCheck();

		GLint shader_ok;
		glGetShaderiv(id, GL_COMPILE_STATUS, out shader_ok);
		if (shader_ok == 0) {
			GLint log_length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, out log_length);
			var log = new uint8[log_length];
			glGetShaderInfoLog(id, log_length, out log_length, log);
			error("Failed to compile shader:\n%s", (string)log);
		}
	}
}

class Oort.VertexShader : Oort.Shader {
	public VertexShader(uint8[] src) {
		base(GL_VERTEX_SHADER, src);
	}
}

class Oort.FragmentShader : Oort.Shader {
	public FragmentShader(uint8[] src) {
		base(GL_FRAGMENT_SHADER, src);
	}
}

class Oort.ShaderProgram {
	public GLuint id;

	VertexShader vertex_shader;
	FragmentShader fragment_shader;

	public ShaderProgram(VertexShader vs, FragmentShader fs) {
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
			warning("Failed to link shader program:\n");
			//show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
		}
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

	public GLint attribute(string name) {
		var x = glGetAttribLocation(id, name);
		glCheck();
		if (x == -1) {
			GLib.error("bad attribute %s", name);
		}
		return x;
	}
}
