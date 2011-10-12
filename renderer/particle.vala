using GL;
using Vector;
using Math;

public enum Oort.ParticleType {
	HIT = 0,
	PLASMA = 1,
	ENGINE = 2,
	EXPLOSION = 3,
}

public struct Oort.Particle {
	public Vec2f initial_position;
	public Vec2f velocity;
	public float initial_time;
	public float lifetime;
	public float type;
	public float padding;
}

public class Oort.ParticleBunch {
	public float initial_time;
	public GLuint buffer;
	public int count;
	Particle[] data;
	static Rand prng = new Rand();
	static GLuint[] buffer_freelist = {};

	public ParticleBunch(float initial_time) {
		this.initial_time = initial_time;
		this.buffer = 0;
		this.count = 0;
		this.data = {};
	}

	public void shower(ParticleType type,
	                   Vec2f p0, Vec2f v0, Vec2f v,
	                   float s_max, float life_min, float life_max,
	                   uint16 count) {
		if (buffer != 0) {
			error("cannot shower after build");
		}

		int i;
		for (i = 0; i < count; i++) {
			float a = (float)prng.next_double() * 3.1416f * 2.0f;
			float s = (float)prng.next_double() * s_max;
			float fdp = (float)prng.next_double();
			var dp = v.scale(fdp*(float)Game.TICK_LENGTH);
			float lifetime = (float)prng.double_range(life_min, life_max);
			var dv = vec2f(cosf(a)*s, sinf(a)*s);
			data += Particle() {
				initial_position = p0.add(dp).add(dv.scale((float)Game.TICK_LENGTH)),
			  velocity = v0.add(v).add(dv),
				initial_time = initial_time,
				lifetime = lifetime,
				type = (float)type
			};
			this.count += 1;
		}
	}

	public void build() {
		if (ParticleBunch.buffer_freelist.length > 0) {
			int i = ParticleBunch.buffer_freelist.length - 1;
			buffer = ParticleBunch.buffer_freelist[i];
			ParticleBunch.buffer_freelist.resize(i);
		} else {
			glGenBuffers(1, &buffer);
		}
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, (GL.GLsizeiptr) (count*sizeof(Particle)), data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glCheck();
		data = null;
	}

	~ParticleBunch() {
		if (buffer != 0) {
			ParticleBunch.buffer_freelist += buffer;
		}
	}
}
