using Oort;
using GL;

class TailProgram : ShaderProgram {
	public GLint u_p_matrix;
	public GLint u_color;
	public GLint a_vertex;
	public GLint a_alpha;

	public TailProgram() throws ShaderError {
		var vs = new VertexShader.from_resource("tail.v.glsl");
		var fs = new FragmentShader.from_resource("tail.f.glsl");
		base("tail", vs, fs);
		u_p_matrix = uniform("p_matrix");
		u_color = uniform("color");
		a_vertex = attribute("vertex");
		a_alpha = attribute("alpha");
	}
}
