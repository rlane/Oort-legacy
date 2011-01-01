using RISC;
using Vector;

public enum RISC.BulletType {
	SLUG = 1,
	PLASMA = 2,
}

[Compact]
public class RISC.Bullet {
	public Physics physics;
	public unowned Team team;
	public double ttl;
	public bool dead;
	public BulletType type;

	public static List<Bullet> all_bullets;
	static List<Bullet> new_bullets;
	static Mutex new_bullets_lock;

	~Bullet() {
		//print("destroy bullet %p\n", this);
	}

	public static void init() {
		new_bullets_lock = new Mutex();
	}

	public static void create(Team team, Vec2 p, Vec2 v, double r, double m, double ttl, BulletType type) {
		var physics = new Physics() { p=p, p0=p, v=v, thrust=vec2(0,0), m=m, r=r };
		var b = new Bullet() { team=team, physics=(owned)physics, ttl=ttl, type=type };

		new_bullets_lock.lock();
		new_bullets.append((owned) b);
		new_bullets_lock.unlock();
	}

	[CCode (cname = "leak")]
	static extern Bullet unleak_bullet(Bullet b);

	public static void purge() {
		unowned List<Bullet> cur = all_bullets;
		while (cur != null) {
			unowned List<Bullet> next = cur.next;
			if (cur.data.dead) {
				unleak_bullet(cur.data);
				all_bullets.delete_link(cur);
			}
			cur = next;
		}
	}

	public static void shutdown() {
		new_bullets_lock = null;
		new_bullets = null;
		all_bullets = null;
	}

	public static void tick() {
		all_bullets.concat((owned) new_bullets);
		new_bullets = null;

		foreach (unowned Bullet b in all_bullets) {
			b.ttl -= Game.TICK_LENGTH;
			if (b.ttl <= 0) {
				b.dead = true;
			}
		}
	}
}
