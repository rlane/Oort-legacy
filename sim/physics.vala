using Oort;
using Vector;

[Compact]
public class Oort.Physics {
	public Vector.Vec2 p;
	public Vector.Vec2 p0;
	public Vector.Vec2 v;
	public Vector.Vec2 acc;
	public double h;
	public double w;
	public double wa;
	public double r;
	public double m;

	public void tick_one() {
		var dv = acc.rotate(h).scale(Game.TICK_LENGTH);
		p0 = p;
		p = p.add(v.add(dv.scale(0.5)).scale(Game.TICK_LENGTH));
		v = v.add(dv);
		h += (w+0.5*wa*Game.TICK_LENGTH)*Game.TICK_LENGTH;
		if (h > Math.PI*2) {
			h -= Math.PI*2;
		} else if (h < 0) {
			h += Math.PI*2;
		}
		w = w + wa*Game.TICK_LENGTH;
	}

	static double collision_time(Physics q1, Physics q2)
	{
		double r_sum = q1.r + q2.r;
		Vec2 dv = q1.v.sub(q2.v);
		Vec2 dp = q1.p.sub(q2.p);
		if (dp.abs() <= r_sum) return 0;
		double a = dv.dot(dv);
		double b = 2*dp.dot(dv);
		double c = dp.dot(dp) - r_sum*r_sum;
		double disc = b*b - 4*a*c;
		if (disc < 0) {
			return double.NAN;
		} else if (disc == 0) {
			return -b/(2*a);
		} else {
			double t0 = (-b - Math.sqrt(disc))/(2*a);
			double t1 = (-b + Math.sqrt(disc))/(2*a);
			if (t0 < 0) return t1;
			if (t1 < 0) return t0;
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

	public Physics() {}

	public Physics copy() {
		var q = new Physics();
		q.p = p;
		q.p0 = p0;
		q.v = v;
		q.acc = acc;
		q.r = r;
		q.m = m;
		q.h = h;
		q.w = w;
		q.wa = wa;
		return q;
	}
}
