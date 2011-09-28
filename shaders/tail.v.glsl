#version 110

uniform mat4 p_matrix;
uniform vec4 color;
attribute vec2 vertex;
attribute float alpha;
varying vec4 v_color;

void main()
{
	v_color = vec4(color.xyz, color.w*alpha);
	gl_Position = p_matrix * vec4(vertex, 0.0, 1.0);
}
