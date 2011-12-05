// Copyright 2011 Rich Lane
#ifndef OORT_RENDERER_RENDERER_H_
#define OORT_RENDERER_RENDERER_H_

#include <memory>
#include <boost/scoped_ptr.hpp>
#include "sim/game.h"
#include "gl/program.h"
#include "gl/buffer.h"

namespace Oort {

class Renderer {
public:
	Renderer(std::shared_ptr<Game> game);
	void render(float view_radius, float aspect_ratio);

private:
	std::shared_ptr<Game> game;
	boost::scoped_ptr<GL::Program> prog;
	GL::Buffer vertex_buf;
};

}

#endif
