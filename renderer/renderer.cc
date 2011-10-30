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

	prog->uniform("p_matrix", p_matrix);

	BOOST_FOREACH(auto ship, game->ships) {
		glm::mat4 mv_matrix;
		glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
		vec2 vertex(ship->physics.p);
		GL::check();

		prog->uniform("mv_matrix", mv_matrix);
		prog->uniform("color", color);
		prog->attrib("vertex", vertex);
		glDrawArrays(GL_POINTS, 0, 1);
		GL::check();
	}

	GL::Program::clear();
	GL::check();
}

}
