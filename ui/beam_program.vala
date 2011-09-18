using Oort;
using GL;

class BeamProgram : ShaderProgram {
	public GLint u_mv_matrix;
	public GLint u_p_matrix;
	public GLint u_color;
	public GLint a_vertex;
	public GLint a_texcoord;

	public BeamProgram() {
		var vs_src = Game.load_resource("shaders/beam.v.glsl");
		var fs_src = Game.load_resource("shaders/beam.f.glsl");
		var vs = new VertexShader(vs_src);
		var fs = new FragmentShader(fs_src);
		base(vs, fs);
		u_mv_matrix = uniform("mv_matrix");
		u_p_matrix = uniform("p_matrix");
		u_color = uniform("color");
		a_vertex = attribute("vertex");
		a_texcoord = attribute("texcoord");
	}
}
