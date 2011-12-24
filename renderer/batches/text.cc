#include "renderer/batches/text.h"

#include <memory>
#include <stdint.h>
#include <boost/foreach.hpp>
#include "gl/texture.h"
#include "gl/check.h"
#include "renderer/font.h"
#include "common/resources.h"

using glm::vec2;
using glm::vec4;
using std::make_shared;
using std::shared_ptr;
using boost::scoped_ptr;

namespace Oort {
namespace RendererBatches {

TextBatch::TextBatch(Renderer &renderer)
	: Batch(renderer),
    prog(GL::Program(
      make_shared<GL::VertexShader>(load_resource("shaders/text.v.glsl")),
      make_shared<GL::FragmentShader>(load_resource("shaders/text.f.glsl"))))
{
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
				uint8_t row = oort_font[8*i+y];
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

void TextBatch::render() {
	BOOST_FOREACH(auto &text, renderer.texts) {
		auto x = text.x;
		auto y = text.y;
		auto &str = text.str;

		auto pos = renderer.pixel2screen(vec2(x,y));
		auto spacing = 9.0f;
		auto n = str.length();

		std::vector<float> data(2*n);
		for (unsigned int i = 0; i < n; i++) {
			data[2*i] = float(str[i]); // character
			data[2*i+1] = float(i); // index
		}

		prog.use();
		font_tex.bind();
		prog.uniform("tex", 0);
		prog.uniform("dist", 2.0f*spacing/renderer.screen_width);
		prog.uniform("position", pos);
		prog.attrib_ptr("character", &data[0], 8);
		prog.attrib_ptr("index", &data[1], 8);
		prog.enable_attrib_array("character");
		prog.enable_attrib_array("index");
		glDrawArrays(GL_POINTS, 0, n);
		prog.disable_attrib_array("character");
		prog.disable_attrib_array("index");
		GL::Texture::unbind();
		GL::Program::clear();
		GL::check();
	}
}

}
}
