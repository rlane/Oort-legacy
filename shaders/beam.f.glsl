#if GL_ES
precision mediump float;
#else
#version 120
#endif

uniform vec4 color;
varying vec2 v_texcoord;

const float PI = 3.141;

void main()
{
	float x = 1.0-v_texcoord.x;
	float y = v_texcoord.y;
	float s = sin(PI*y);
	float a = clamp(0.7*x + 0.5*x*abs(sin(PI/2.0+20.0*x)), 0.0, 1.0);
	gl_FragColor = vec4(color.xyz, s*a);
}
