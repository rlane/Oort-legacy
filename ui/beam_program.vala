using Oort;
using GL;

class BeamProgram : ShaderProgram {
	public GLint u_mv_matrix;
	public GLint u_p_matrix;
	public GLint u_color;
	public GLint a_vertex;
	public GLint a_texcoord;

	public BeamProgram() throws ShaderError {
		var vs = new VertexShader.from_resource("beam.v.glsl");
		var fs = new FragmentShader.from_resource("beam.f.glsl");
		base("beam", vs, fs);
		u_mv_matrix = uniform("mv_matrix");
		u_p_matrix = uniform("p_matrix");
		u_color = uniform("color");
		a_vertex = attribute("vertex");
		a_texcoord = attribute("texcoord");
	}
}
