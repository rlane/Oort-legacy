// Copyright 2011 Rich Lane
#ifndef OORT_GL_PROGRAM_H_
#define OORT_GL_PROGRAM_H_

#include <stdio.h>
#include <GL/glew.h>
#include <memory>
#include <unordered_map>
#include "glm/gtc/type_ptr.hpp"
#include "common/log.h"
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
		auto loc_iter = uniform_locs.find(name);
		if (loc_iter != uniform_locs.end()) {
			return loc_iter->second;
		}
		int loc = glGetUniformLocation(id, name.c_str());
		uniform_locs.insert(LocMap::value_type(name, loc));
		return loc;
	}

	int attrib_location(std::string name) {
		auto loc_iter = attrib_locs.find(name);
		if (loc_iter != attrib_locs.end()) {
			return loc_iter->second;
		}
		int loc = glGetAttribLocation(id, name.c_str());
		attrib_locs.insert(LocMap::value_type(name, loc));
		return loc;
	}

	void uniform(std::string name, int val) {
		glUniform1i(uniform_location(name), val);
	}

	void uniform(std::string name, float val) {
		glUniform1f(uniform_location(name), val);
	}

	void uniform(std::string name, const glm::vec2 &val) {
		glUniform2fv(uniform_location(name), 1, glm::value_ptr(val));
	}

	void uniform(std::string name, const glm::vec4 &val) {
		glUniform4fv(uniform_location(name), 1, glm::value_ptr(val));
	}

	void uniform(std::string name, const glm::mat4 &val) {
		glUniformMatrix4fv(uniform_location(name), 1, false, glm::value_ptr(val));
	}

	void attrib(std::string name, const glm::vec2 &val) {
		glVertexAttrib2f(attrib_location(name), val.x, val.y);
	}

	void attrib_ptr(std::string name, const float *vals, int stride=0) {
		glVertexAttribPointer(attrib_location(name), 1, GL_FLOAT, false, stride, vals);
	}

	void attrib_ptr(std::string name, const glm::vec2 *vals, int stride=0) {
		glVertexAttribPointer(attrib_location(name), 2, GL_FLOAT, false, stride, vals);
	}

	void attrib_ptr(std::string name, const glm::vec4 *vals, int stride=0) {
		glVertexAttribPointer(attrib_location(name), 4, GL_FLOAT, false, stride, vals);
	}

	void enable_attrib_array(std::string name) {
		glEnableVertexAttribArray(attrib_location(name));
	}

	void disable_attrib_array(std::string name) {
		glDisableVertexAttribArray(attrib_location(name));
	}

private:
	typedef std::unordered_map<std::string,int> LocMap;
	LocMap attrib_locs;
	LocMap uniform_locs;
};

}

#endif
