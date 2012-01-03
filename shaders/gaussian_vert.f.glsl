// http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
#version 120

uniform sampler2D tex;
uniform float screen_height_inv;
varying vec2 coord;

uniform float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
uniform float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

void main(void)
{
	gl_FragColor = texture2D( tex, coord ) * weight[0];
	for (int i=1; i<3; i++) {
		gl_FragColor += texture2D( tex, coord+vec2(0.0, offset[i]*screen_height_inv) ) * weight[i];
		gl_FragColor += texture2D( tex, coord-vec2(0.0, offset[i]*screen_height_inv) ) * weight[i];
	}
}
