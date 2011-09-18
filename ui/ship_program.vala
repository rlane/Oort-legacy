using Oort;
using GL;

class ShipProgram : ShaderProgram {
	public GLint u_mv_matrix;
	public GLint u_p_matrix;
	public GLint u_color;
	public GLint a_vertex;

	public ShipProgram() throws ShaderError {
		var vs = new VertexShader.from_resource("ship.v.glsl");
		var fs = new FragmentShader.from_resource("ship.f.glsl");
		base("ship", vs, fs);
		u_mv_matrix = uniform("mv_matrix");
		u_p_matrix = uniform("p_matrix");
		u_color = uniform("color");
		a_vertex = attribute("vertex");
	}
}
