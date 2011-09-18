#version 120

uniform vec4 color;
varying vec2 v_texcoord;

const float PI = 3.141;

void main()
{
	float s = sin(PI*v_texcoord.y);
	float a = v_texcoord.x;
	gl_FragColor = vec4(color.xyz, s*(1-a));
}
