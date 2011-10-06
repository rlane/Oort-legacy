uniform mat4 mv_matrix;
uniform mat4 p_matrix;
attribute vec2 vertex;

void main()
{
	gl_Position = p_matrix * mv_matrix * vec4(vertex, 0.0, 1.0);
}
