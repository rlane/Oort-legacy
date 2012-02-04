uniform mat4 p_matrix;
uniform float initial_time;
uniform float current_time;
attribute vec2 vertex;
attribute vec4 color;
varying vec4 v_color;

void main()
{
	float alpha = max(0.0, color.a - 0.02 * (current_time - initial_time));
	v_color = vec4(color.rgb, alpha);
	gl_Position = p_matrix * vec4(vertex, 0.0, 1.0);
}
