#pragma once

namespace GL {

using std::shared_ptr;

class Shader {
public:
	typedef shared_ptr<Shader> P;

	GLuint id;

	Shader(GLenum type, std::string code)
	{
		id = glCreateShader(type);
		const GLchar *sources[1] = { (GLchar*) code.data() };
		GLint source_lens[1] = { (GLint) code.size() };
		glShaderSource(id, 1, sources, source_lens);
		glCompileShader(id);
		GL::check();

		display_info_log();

		int status;
		glGetShaderiv(id, GL_COMPILE_STATUS, &status);
		GL::check();

		if (status == GL_FALSE) {
			throw new std::exception();
		}
	}

	void display_info_log() {
		int len;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
		GL::check();
		if (len > 1) {
			auto log = new char[len];
			glGetShaderInfoLog(id, len, &len, log);
			std::cerr << "shader info log: " << log;
			delete[] log;
		}
	}

	~Shader()
	{
		if (id != 0) {
			glDeleteShader(id);
		}
	}

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
};

class FragmentShader : public Shader {
public:
	FragmentShader(std::string filename) :
		Shader(GL_FRAGMENT_SHADER, filename) {}
};

class VertexShader : public Shader {
public:
	VertexShader(std::string filename) :
		Shader(GL_VERTEX_SHADER, filename) {}
};

}
