// Copyright 2011 Rich Lane
#ifndef OORT_GL_PROGRAM_H_
#define OORT_GL_PROGRAM_H_

#include <stdio.h>
#include <GL/glew.h>
#include <memory>
#include "gl/shader.h"
#include "gl/check.h"

namespace GL {

using std::shared_ptr;

class Program {
public:
	GLuint id;
	shared_ptr<VertexShader> vertex_shader;
	shared_ptr<FragmentShader> fragment_shader;

	Program(shared_ptr<VertexShader> vertex_shader,
	        shared_ptr<FragmentShader> fragment_shader) :
	        vertex_shader(vertex_shader),
	        fragment_shader(fragment_shader) {
		id = glCreateProgram();
		glAttachShader(id, vertex_shader->id);
		glAttachShader(id, fragment_shader->id);
		glLinkProgram(id);
		display_info_log();
		GL::check();
		glValidateProgram(id);
		int status;
		glGetProgramiv(id, GL_VALIDATE_STATUS, &status);
		GL::check();

		if (status == GL_FALSE) {
			throw new std::exception();
		}
	}

	void display_info_log() {
		int len;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
		GL::check();
		if (len > 1) {
			auto log = new char[len];
			glGetProgramInfoLog(id, len, &len, log);
			fprintf(stderr, "program info log: %s\n", log);
			delete[] log;
		}
	}

	~Program() {
		glDeleteProgram(id);
	}

	void use() {
		GL::check();
		glUseProgram(id);
		GL::check();
	}

	static void clear() {
		GL::check();
		glUseProgram(0);
		GL::check();
	}

	int uniform_location(std::string name) {
		return glGetUniformLocation(id, name.c_str());
	}

	int attrib_location(std::string name) {
		return glGetAttribLocation(id, name.c_str());
	}
};

}

#endif
