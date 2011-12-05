#include "renderer/renderer.h"
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include <Box2D/Box2D.h>

#include "sim/ship.h"
#include "sim/bullet.h"
#include "sim/team.h"
#include "common/log.h"
#include "common/resources.h"
#include "gl/program.h"
#include "gl/shader.h"
#include "gl/buffer.h"
#include "gl/check.h"

using glm::vec2;
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
	std::vector<vec2> vertices = { vec2(-0.7, -0.71),
	                               vec2(-0.7, 0.71),
	                               vec2(1, 0) };

	vertex_buf.data(vertices);
}

void Renderer::render(float view_radius,
                      float aspect_ratio,
                      glm::vec2 view_center) {
	GL::check();
	glClear(GL_COLOR_BUFFER_BIT);
	prog->use();
	glEnableVertexAttribArray(prog->attrib_location("vertex"));
	GL::check();

	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glLineWidth(1.2f);

	glm::mat4 p_matrix = glm::ortho(view_center.x - view_radius,
	                                view_center.x + view_radius,
	                                view_center.y - view_radius/aspect_ratio,
	                                view_center.y + view_radius/aspect_ratio);

	prog->uniform("p_matrix", p_matrix);

	BOOST_FOREACH(auto ship, game->ships) {
		glm::mat4 mv_matrix;
		auto p = ship->body->GetPosition();
		auto h = ship->body->GetAngle();
		mv_matrix = glm::translate(mv_matrix, glm::vec3(p.x, p.y, 0));
		mv_matrix = glm::rotate(mv_matrix, glm::degrees(h), glm::vec3(0, 0, 1));
		glm::vec4 color(ship->team->color, 0.7f);
		GL::check();

		prog->uniform("mv_matrix", mv_matrix);
		prog->uniform("color", color);
		vertex_buf.bind();
		glVertexAttribPointer(prog->attrib_location("vertex"),
		                      2, GL_FLOAT, GL_FALSE, 0, 0);
		vertex_buf.unbind();
		GL::check();

		glDrawArrays(GL_LINE_LOOP, 0, 3);
		GL::check();
	}

	BOOST_FOREACH(auto bullet, game->bullets) {
		glm::mat4 mv_matrix;
		auto p = bullet->body->GetPosition();
		auto h = bullet->body->GetAngle();
		mv_matrix = glm::translate(mv_matrix, glm::vec3(p.x, p.y, 0));
		mv_matrix = glm::rotate(mv_matrix, glm::degrees(h), glm::vec3(0, 0, 1));
		mv_matrix = glm::scale(mv_matrix, glm::vec3(0.1f, 0.1f, 0.1f));
		glm::vec4 color(bullet->team->color, 0.4f);
		GL::check();

		prog->uniform("mv_matrix", mv_matrix);
		prog->uniform("color", color);
		vertex_buf.bind();
		glVertexAttribPointer(prog->attrib_location("vertex"),
		                      2, GL_FLOAT, GL_FALSE, 0, 0);
		vertex_buf.unbind();
		GL::check();

		glDrawArrays(GL_LINE_LOOP, 0, 3);
		GL::check();
	}


	glDisableVertexAttribArray(prog->attrib_location("vertex"));
	GL::Program::clear();
	GL::check();
}

}
