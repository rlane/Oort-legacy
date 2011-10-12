uniform mat4 p_matrix;
uniform float current_time;
uniform float view_scale;
attribute vec2 initial_position;
attribute vec2 velocity;
attribute float initial_time;
attribute float lifetime;
attribute float type;
varying vec4 v_color;

#define PARTICLE_TYPE_HIT 0.0
#define PARTICLE_TYPE_PLASMA 1.0
#define PARTICLE_TYPE_ENGINE 2.0
#define PARTICLE_TYPE_EXPLOSION 3.0

void main(void) {
	float time_left = initial_time + lifetime - current_time;
	if (current_time < initial_time || time_left < 0.0) {
		gl_PointSize = 0.0;
		gl_Position = vec4(0, 0, 0, 0);
		v_color = vec4(0.0, 0.0, 0.0, 0.0);
	} else {
		float size;
		vec4 color;
		if (type == PARTICLE_TYPE_HIT) {
			size = 96.0*time_left;
			color = vec4(1.0, 0.78, 0.78, time_left);
		} else if (type == PARTICLE_TYPE_PLASMA) {
			size = 128.0*time_left;
			color = vec4(1.0, 0.1, 0.1, 4.0*time_left);
		} else if (type == PARTICLE_TYPE_ENGINE) {
			size = 32.0*time_left;
			color = vec4(1.0, 0.8, 0.17, 0.039 + 0.64*time_left);
		} else if (type == PARTICLE_TYPE_EXPLOSION) {
			float s = length(velocity);
			size = 0.5 + 16.0*time_left;
			float g = min(1.0, 0.0625*s + 3.2*time_left);
			color = vec4(1.0, g, 0.2, 0.04 + 2.496*time_left);
		}
		v_color = color;
		gl_PointSize = size*view_scale;
		float t = current_time - initial_time;
		vec2 position = initial_position + t*velocity;
		gl_Position = p_matrix * vec4(position, 0.0, 1.0);
	}
}
