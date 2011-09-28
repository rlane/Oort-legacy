#version 110

uniform mat4 p_matrix;
uniform mat4 mv_matrix;
attribute vec2 vertex;
attribute vec2 texcoord;
varying vec2 v_texcoord;

void main(void) {
	v_texcoord = texcoord;
	gl_Position = p_matrix * mv_matrix * vec4(vertex, 0.0, 1.0);
}
