attribute vec2 vertex;
varying vec2 coord;

void main()
{
	gl_Position = vec4(vertex*2.0-vec2(1.0,1.0), 0.0, 1.0);
	coord = vertex;
}
