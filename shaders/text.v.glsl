#version 110

uniform vec2 position;
uniform float dist;
attribute float index;
attribute float character;
varying float v_character;

void main(void) {
	v_character = character;
	gl_PointSize = 8.0;
	gl_Position = vec4(position+vec2(dist*index,0), 0.0, 1.0);
}
