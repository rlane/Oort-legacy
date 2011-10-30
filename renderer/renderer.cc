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
	GL::check();

	glEnable(GL_PROGRAM_POINT_SIZE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glm::mat4 p_matrix = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f);

	BOOST_FOREACH(auto ship, game->ships) {
		glm::mat4 mv_matrix;
		glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
		vec2 vertex(ship->physics.p);

		int mv_matrix_loc = glGetUniformLocation(prog->id, "mv_matrix");
		int p_matrix_loc = glGetUniformLocation(prog->id, "p_matrix");
		int color_loc = glGetUniformLocation(prog->id, "color");
		int vertex_loc = glGetAttribLocation(prog->id, "vertex");
		GL::check();

		glUniformMatrix4fv(mv_matrix_loc, 1, false, glm::value_ptr(mv_matrix));
		glUniformMatrix4fv(p_matrix_loc, 1, false, glm::value_ptr(p_matrix));
		glUniform4fv(color_loc, 1, glm::value_ptr(color));
		glVertexAttrib2f(vertex_loc, vertex.x, vertex.y);
		glDrawArrays(GL_POINTS, 0, 1);
		GL::check();
	}

	GL::Program::clear();
	GL::check();
}

}
