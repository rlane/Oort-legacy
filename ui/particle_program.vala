using Oort;
using GL;

class ParticleProgram : ShaderProgram {
	public GLint u_p_matrix;
	public GLint u_color;
	public GLint a_position;

	public ParticleProgram() throws ShaderError {
		var vs = new VertexShader.from_resource("particle.v.glsl");
		var fs = new FragmentShader.from_resource("particle.f.glsl");
		base("particle", vs, fs);
		u_p_matrix = uniform("p_matrix");
		u_color = uniform("color");
		a_position = attribute("position");
	}
}
