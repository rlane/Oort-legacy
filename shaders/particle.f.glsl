#if !GL_ES
#version 120
#endif

varying vec4 v_color;
uniform sampler2D tex;

void main()
{
	float alpha = texture2D(tex, gl_PointCoord).a;
	gl_FragColor = vec4(v_color.xyz, 0) * alpha * v_color.w;
}
