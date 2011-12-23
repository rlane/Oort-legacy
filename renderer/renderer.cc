#include "renderer/renderer.h"
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <stdint.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include <Box2D/Box2D.h>

#include "sim/ship.h"
#include "sim/ship_class.h"
#include "sim/bullet.h"
#include "sim/beam.h"
#include "sim/team.h"
#include "sim/model.h"
#include "sim/math_util.h"
#include "common/log.h"
#include "common/resources.h"
#include "gl/program.h"
#include "gl/shader.h"
#include "gl/buffer.h"
#include "gl/texture.h"
#include "gl/check.h"
#include "renderer/font.h"

using glm::vec2;
using std::make_shared;
using std::shared_ptr;
using boost::scoped_ptr;

namespace Oort {

Renderer::Renderer(shared_ptr<Game> game)
  : game(game),
    ship_prog(new GL::Program(
      make_shared<GL::VertexShader>(load_resource("shaders/ship.v.glsl")),
      make_shared<GL::FragmentShader>(load_resource("shaders/ship.f.glsl")))),
    bullet_prog(new GL::Program(
      make_shared<GL::VertexShader>(load_resource("shaders/bullet.v.glsl")),
      make_shared<GL::FragmentShader>(load_resource("shaders/bullet.f.glsl")))),
    text_prog(new GL::Program(
      make_shared<GL::VertexShader>(load_resource("shaders/text.v.glsl")),
      make_shared<GL::FragmentShader>(load_resource("shaders/text.f.glsl"))))
{
	load_font();
}

void Renderer::load_font() {
	font_tex.bind();
	GL::check();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GL::check();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GL::check();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GL::check();
	const int n = 256;
	unsigned char data[64*n];
	for (int i = 0; i < n; i++) {
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				uint8 row = oort_font[8*i+y];
				bool on = ((row >> x) & 1) == 1;
				data[n*8*y + 8*i + x] = on ? 255 : 0;
			}
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, n*8, 8, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
	GL::check();
	GL::Texture::unbind();
	GL::check();
}

void Renderer::reshape(int screen_width, int screen_height) {
	this->screen_width = screen_width;
	this->screen_height = screen_height;
	this->aspect_ratio = float(screen_width)/screen_height;
}

void Renderer::render(float view_radius,
                      glm::vec2 view_center) {
	GL::check();

	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.03f, 0.0f);
	glLineWidth(1.2f);

	glClear(GL_COLOR_BUFFER_BIT);

	p_matrix = glm::ortho(view_center.x - view_radius,
	                      view_center.x + view_radius,
	                      view_center.y - view_radius/aspect_ratio,
	                      view_center.y + view_radius/aspect_ratio);

	render_ships();
	render_bullets();
	render_beams();
}

void Renderer::render_ships() {
	ship_prog->use();
	ship_prog->enable_attrib_array("vertex");
	GL::check();
	ship_prog->uniform("p_matrix", p_matrix);

	BOOST_FOREACH(auto ship, game->ships) {
		if (ship->dead) {
			continue;
		}

		glm::mat4 mv_matrix;
		auto p = ship->get_position();
		auto h = ship->get_heading();
		mv_matrix = glm::translate(mv_matrix, glm::vec3(p.x, p.y, 0));
		mv_matrix = glm::rotate(mv_matrix, glm::degrees(h), glm::vec3(0, 0, 1));
		mv_matrix = glm::scale(mv_matrix, glm::vec3(1, 1, 1) * ship->klass.scale);
		glm::vec4 color(ship->team->color, ship->klass.model->alpha);
		GL::check();

		ship_prog->uniform("mv_matrix", mv_matrix);
		ship_prog->uniform("color", color);

		BOOST_FOREACH(Shape &shape, ship->klass.model->shapes) {
			GL::Buffer *&vertex_buf = shape.vertex_buffer;
			if (!vertex_buf) {
				vertex_buf = new GL::Buffer();
				vertex_buf->data(shape.vertices);
			}
			vertex_buf->bind();
			glVertexAttribPointer(ship_prog->attrib_location("vertex"),
			                      2, GL_FLOAT, GL_FALSE, 0, 0);
			vertex_buf->unbind();
			GL::check();

			glDrawArrays(GL_LINE_LOOP, 0, shape.vertices.size());
			GL::check();
		}
	}

	ship_prog->disable_attrib_array("vertex");
	GL::Program::clear();
	GL::check();
}

void Renderer::render_bullets() {
	boost::random::mt19937 prng(game->ticks);
	boost::random::normal_distribution<> p_dist(0.0, 0.5);

	glm::vec4 colors[] = {
		glm::vec4(0.27f, 0.27f, 0.27f, 0.33f),
		glm::vec4(0.27f, 0.27f, 0.27f, 1.0f)
	};

	bullet_prog->use();
	GL::check();

	bullet_prog->enable_attrib_array("vertex");
	bullet_prog->enable_attrib_array("color");
	bullet_prog->uniform("p_matrix", p_matrix);
	bullet_prog->uniform("mv_matrix", glm::mat4());

	glVertexAttribPointer(bullet_prog->attrib_location("color"), 4, GL_FLOAT, false, 0, colors);

	BOOST_FOREACH(auto bullet, game->bullets) {
		if (bullet->dead) {
			continue;
		}

		auto dp = bullet->get_velocity() * (1.0f/40);
		auto p1 = bullet->get_position() - dp;
		auto p2 = bullet->get_position();

		glm::vec2 vertices[2] = { p1, p2 };
		glVertexAttribPointer(bullet_prog->attrib_location("vertex"), 2, GL_FLOAT, false, 0, vertices);
		glDrawArrays(GL_LINES, 0, 2);
	}

	bullet_prog->disable_attrib_array("vertex");
	bullet_prog->disable_attrib_array("color");
	GL::Program::clear();
	GL::check();
}

void Renderer::render_beams() {
	glm::vec4 colors[] = {
		glm::vec4(0.27f, 0.27f, 0.8f, 0.6f),
		glm::vec4(0.27f, 0.27f, 0.8f, 0.6f),
		glm::vec4(0.27f, 0.27f, 0.8f, 0.6f),
		glm::vec4(0.27f, 0.27f, 0.8f, 0.6f),
	};

	bullet_prog->use();
	GL::check();

	bullet_prog->enable_attrib_array("vertex");
	bullet_prog->enable_attrib_array("color");
	bullet_prog->uniform("p_matrix", p_matrix);

	glVertexAttribPointer(bullet_prog->attrib_location("color"), 4, GL_FLOAT, false, 0, colors);

	BOOST_FOREACH(auto beam, game->beams) {
		auto p = beam->get_position();
		auto h = beam->get_heading();

		glm::mat4 mv_matrix;
		mv_matrix = glm::translate(mv_matrix, glm::vec3(p.x, p.y, 0));
		mv_matrix = glm::rotate(mv_matrix, glm::degrees(h), glm::vec3(0, 0, 1));
		bullet_prog->uniform("mv_matrix", mv_matrix);

		const BeamDef &def = beam->get_def();
		glm::vec2 vertices[] = {
			vec2(0, -def.width/2),
			vec2(def.length, -def.width/2),
			vec2(def.length, def.width/2),
			vec2(0, def.width/2),
		};

		glVertexAttribPointer(bullet_prog->attrib_location("vertex"), 2, GL_FLOAT, false, 0, vertices);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	bullet_prog->disable_attrib_array("vertex");
	bullet_prog->disable_attrib_array("color");
	GL::Program::clear();
	GL::check();
}

// XXX
static constexpr float screen_width = 1600;
static constexpr float screen_height = 900;

static vec2 pixel2screen(vec2 p) {
	return vec2((float) (2*p.x/screen_width-1),
	            (float) (-2*p.y/screen_height+1));
}

void Renderer::text(int x, int y, const std::string &str) {
	auto pos = pixel2screen(vec2(x,y));
	auto spacing = 9.0f;
	auto n = str.length();

	std::vector<float> data(2*n);
	for (unsigned int i = 0; i < n; i++) {
		data[2*i] = float(str[i]); // character
		data[2*i+1] = float(i); // index
	}

	text_prog->use();
	font_tex.bind();
	text_prog->uniform("tex", 0);
	text_prog->uniform("dist", 2.0f*spacing/screen_width);
	text_prog->uniform("position", pos);
	text_prog->attrib_ptr("character", &data[0], 8);
	text_prog->attrib_ptr("index", &data[1], 8);
	text_prog->enable_attrib_array("character");
	text_prog->enable_attrib_array("index");
	glDrawArrays(GL_POINTS, 0, n);
	text_prog->disable_attrib_array("character");
	text_prog->disable_attrib_array("index");
	GL::Texture::unbind();
	GL::Program::clear();
	GL::check();
}

}
