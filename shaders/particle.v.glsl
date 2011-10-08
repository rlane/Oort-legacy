uniform mat4 p_matrix;
uniform float current_time;
uniform float view_scale;
attribute vec2 initial_position;
attribute vec2 velocity;
attribute float initial_time;
attribute float lifetime;
attribute float type;
varying vec4 v_color;

#define PARTICLE_TYPE_HIT 0
#define PARTICLE_TYPE_PLASMA 1
#define PARTICLE_TYPE_ENGINE 2
#define PARTICLE_TYPE_EXPLOSION 3

void main(void) {
	float time_left = initial_time + lifetime - current_time;
	if (current_time < initial_time || time_left < 0.0) {
		gl_PointSize = 0.0;
		gl_Position = vec4(0, 0, 0, 0);
		v_color = vec4(0.0, 0.0, 0.0, 0.0);
	} else {
		float size;
		vec4 color;
		float ticks_left = time_left * 32.0; // XXX
		if (type == PARTICLE_TYPE_HIT) {
			size = 0.3*ticks_left;
			color = vec4(1.0, 0.78, 0.78, ticks_left*0.03125);
		} else if (type == PARTICLE_TYPE_PLASMA) {
			size = 0.4*ticks_left;
			color = vec4(1.0, 0.1, 0.1, ticks_left*0.125);
		} else if (type == PARTICLE_TYPE_ENGINE) {
			size = 0.1*ticks_left;
			color = vec4(1.0, 0.8, 0.17, 0.039 + ticks_left*0.02);
		} else if (type == PARTICLE_TYPE_EXPLOSION) {
			float s = length(velocity);
			size = 0.05 + 0.05*ticks_left;
			float g = 255.0*min(1.0, 0.0625*s+ticks_left*0.1)/256.0;
			color = vec4(1.0, g, 0.2, 0.04 + ticks_left*0.078);
		}
		v_color = color;
		gl_PointSize = size*10.0*view_scale; // XXX
		float t = current_time - initial_time;
		vec2 position = initial_position + t*velocity*32.0; // XXX
		gl_Position = p_matrix * vec4(position, 0.0, 1.0);
	}
}
