#include "renderer/renderer.h"
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

#include "common/log.h"
#include "common/resources.h"
#include "gl/program.h"
#include "gl/shader.h"
#include "gl/check.h"

using glm::vec2;
using glm::dvec2;
using std::make_shared;
using std::shared_ptr;
using boost::scoped_ptr;

namespace Oort {

Renderer::Renderer(shared_ptr<Game> game)
	: game(game),
	  prog(new GL::Program(
	    make_shared<GL::VertexShader>(load_resource("shaders/ship.v.glsl")),
	    make_shared<GL::FragmentShader>(load_resource("shaders/ship.f.glsl"))))
{
}

void Renderer::render() {
	GL::check();
	glClear(GL_COLOR_BUFFER_BIT);
	prog->use();
	glEnableVertexAttribArray(prog->attrib_location("vertex"));
	GL::check();

	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glm::mat4 p_matrix = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f);

	prog->uniform("p_matrix", p_matrix);

	std::vector<vec2> vertices = { vec2(-0.7, -0.71),
	                               vec2(-0.7, 0.71),
	                               vec2(1, 0) };

	BOOST_FOREACH(auto ship, game->ships) {
		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(ship->physics.p, 0));
		glm::vec4 color(1.0f, 1.0f, 1.0f, 0.3f);
		vec2 vertex(ship->physics.p);
		GL::check();

		prog->uniform("mv_matrix", mv_matrix);
		prog->uniform("color", color);
		glVertexAttribPointer(prog->attrib_location("vertex"),
		                      2, GL_FLOAT, GL_FALSE, 0, &vertices[0]);
		GL::check();

		glDrawArrays(GL_LINE_LOOP, 0, 3);
		GL::check();
	}


	glDisableVertexAttribArray(prog->attrib_location("vertex"));
	GL::Program::clear();
	GL::check();
}

}
