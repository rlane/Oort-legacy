#version 120

uniform vec4 color;

void main()
{
	float alpha = pow(1-clamp(2*distance(gl_PointCoord, vec2(0.5, 0.5)), 0, 1), 2);
	gl_FragColor = vec4(color.xyz, 0) * alpha * color.w;
}
