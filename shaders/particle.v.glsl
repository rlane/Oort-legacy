#version 110

uniform mat4 p_matrix;
attribute vec2 position;

void main(void) {
	gl_Position = p_matrix * vec4(position, 0.0, 1.0);
}
