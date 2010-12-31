using RISC;
using Vector;

[Compact]
public class RISC.Physics {
	public Vector.Vec2 p;
	public Vector.Vec2 p0;
	public Vector.Vec2 v;
	public Vector.Vec2 thrust;
	public double a;
	public double av;
	public double r;
	public double m;

	public void tick_one(double t) {
		var acc = thrust.scale(t/m);
		p0 = p;
		p = p.add(v.add(acc.scale(0.5)).scale(t));
		v = v.add(acc);
	}

	static List<unowned Physics> all_physics;
	static List<unowned Physics> new_physics;
	static Mutex new_physics_lock;

	public static void init() {
		new_physics_lock = new Mutex();
	}

	public static Physics create(Vec2 p, Vec2 p0, Vec2 v, Vec2 thrust,
			                         double a, double av, double m, double r) {
		var q = new Physics() { p=p, p0=p0, v=v, thrust=thrust, a=a, av=av, m=m, r=r };
		new_physics_lock.lock();
		new_physics.append(q);
		new_physics_lock.unlock();
		return q;
	}

	public static void tick(double tick_length) {
		all_physics.concat((owned)new_physics);
		foreach (unowned Physics q in all_physics) { q.tick_one(tick_length); }
	}

	static double collision_time(Physics q1, Physics q2)
	{
		Vec2 dv = q1.v.sub(q2.v);
		Vec2 dp = q1.p.sub(q2.p);
		double a = dv.dot(dv);
		double b = 2*dp.dot(dv);
		double r_sum = q1.r + q2.r;
		double c = dp.dot(dp) - r_sum*r_sum;
		double disc = b*b - 4*a*c;
		if (disc < 0) {
			return double.NAN;
		} else if (disc == 0) {
			return -b/(2*a);
		} else {
			double t0 = (-b - Math.sqrt(disc))/(2*a);
			double t1 = (-b + Math.sqrt(disc))/(2*a);
			return double.min(t0, t1);
		}
	}

	public static bool check_collision(Physics q1, Physics q2, double interval, out Vector.Vec2 rcp) {
		double t = collision_time(q1, q2);
		if (t.is_nan()) {
			return false;
		} if (t < 0) {
			// past collision
			return false;
		} if (t > interval) {
			// future collision
			return false;
		} else {
			rcp = q2.p.add(q2.v.scale(t));
			return true;
		}
	}

	~Physics() {
		//print("destroy physics %p\n", this);
		all_physics.remove(this);
	}

	public Physics() {}

	public Physics copy() {
		var q = new Physics();
		q.p = p;
		q.p0 = p0;
		q.v = v;
		q.thrust = thrust;
		q.a = a;
		q.av = av;
		q.r = r;
		q.m = m;
		return q;
	}
}
