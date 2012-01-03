uniform sampler2D image;

void main()
{
	gl_FragColor = texture2D(tex, gl_FragCoord.xy);
}
