using RISC;
using Vector;

public enum RISC.BulletType {
	SLUG = 1,
	PLASMA = 2,
}

public class RISC.Bullet {
	public Physics physics;
	public unowned Team team;
	public double ttl;
	public bool dead;
	public BulletType type;

	public static List<Bullet> all_bullets;
	static List<Bullet> new_bullets;
	static Mutex new_bullets_lock;

	public static void init() {
		new_bullets_lock = new Mutex();
	}

	public static void create(Team team, Vec2 p, Vec2 v, double r, double m, double ttl, BulletType type) {
		var physics = Physics.create();
		physics.p = p;
		physics.v = v;
		physics.r = r;
		physics.m = m;
		var b = new Bullet() { team=team, physics=(owned)physics, ttl=ttl, type=type };

		new_bullets_lock.lock();
		new_bullets.append((owned) b);
		new_bullets_lock.unlock();
	}

	// XXX
	public static void purge() {
		var dead_bullets = new List<Bullet>();
		foreach (unowned Bullet b in all_bullets) {
			if (b.dead) {
				dead_bullets.prepend(b);
			}
		}

		foreach (unowned Bullet b in dead_bullets) {
			all_bullets.remove(b);
		}
	}

	public static void shutdown() {
		new_bullets_lock = null;
		new_bullets = null;
		all_bullets = null;
	}

	public static void tick(double t) {
		all_bullets.concat((owned) new_bullets);
		new_bullets = null;

		foreach (unowned Bullet b in all_bullets) {
			b.ttl -= t;
			if (b.ttl <= 0) {
				b.dead = true;
			}
		}
	}
}
