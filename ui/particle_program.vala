using Oort;
using GL;

class ParticleProgram : ShaderProgram {
	public GLint u_p_matrix;
	public GLint u_color;
	public GLint a_position;

	public ParticleProgram() {
		var vs_src = Game.load_resource("shaders/particle.v.glsl");
		var fs_src = Game.load_resource("shaders/particle.f.glsl");
		var vs = new VertexShader(vs_src);
		var fs = new FragmentShader(fs_src);
		base(vs, fs);
		u_p_matrix = uniform("p_matrix");
		u_color = uniform("color");
		a_position = attribute("position");
	}
}
