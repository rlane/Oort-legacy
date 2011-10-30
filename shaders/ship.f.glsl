#if GL_ES
precision mediump float;
#else
#version 120
#endif

uniform vec4 color;

void main()
{
  float r = length(gl_PointCoord-vec2(0.5,0.5));
  float alpha = pow(clamp(1-2*r, 0, 1), 2);
  gl_FragColor = vec4(color.rgb, 0) * alpha * color.a;
}
