#pragma once

namespace GL {

void check()
{
	auto err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cerr << "GL error: " << glewGetErrorString(err) << std::endl;
		throw new std::exception();
	}
}

}
