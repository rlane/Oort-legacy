// Copyright 2011 Rich Lane
#ifndef OORT_GL_SHADER_H_
#define OORT_GL_SHADER_H_

#include <stdio.h>
#include <string>

namespace GL {

using std::shared_ptr;

class Shader {
public:
	typedef shared_ptr<Shader> P;

	GLuint id;

	Shader(GLenum type, std::string code) {
		id = glCreateShader(type);
		const GLchar *sources[1] = { reinterpret_cast<const GLchar*>(code.data()) };
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
			fprintf(stderr, "shader info log: %s\n", log);
			delete[] log;
		}
	}

	~Shader() {
		if (id != 0) {
			glDeleteShader(id);
		}
	}

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
};

class FragmentShader : public Shader {
public:
	explicit FragmentShader(std::string filename) :
		Shader(GL_FRAGMENT_SHADER, filename) {}
};

class VertexShader : public Shader {
public:
	explicit VertexShader(std::string filename) :
		Shader(GL_VERTEX_SHADER, filename) {}
};

}

#endif
