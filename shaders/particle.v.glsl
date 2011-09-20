#version 110

uniform mat4 p_matrix;
uniform float current_time;
attribute float initial_time;
attribute float lifetime;
attribute vec2 initial_position;
attribute vec2 velocity;
attribute vec4 color;
varying vec4 v_color;

void main(void) {
	v_color = color;
	if (current_time < initial_time || current_time > (initial_time + lifetime)) {
		gl_Position = vec4(0, 0, 0, 0);
	} else {
		float t = current_time - initial_time;
		vec2 position = initial_position + t*velocity;
		gl_Position = p_matrix * vec4(position, 0.0, 1.0);
	}
}
