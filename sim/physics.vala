using Oort;
using Vector;
using Math;

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

	public static bool solve_quadratic(double a, double b, double c, out double x1, out double x2) {
		var disc = b*b - 4*a*c;
		if (disc < 0) {
			return false;
		} else {
			x1 = (-b + Math.sqrt(disc))/(2*a);
			x2 = (-b - Math.sqrt(disc))/(2*a);
			return true;
		}
	}

	public static bool check_quadrant(double a, double x, double y) {
		if (x >= 0 && y >= 0) {
			return a >= 0 && a <= PI/2;
		} else if (x < 0 && y >= 0) {
			return a >= PI/2 && a <= PI;
		} else if (x < 0 && y < 0) {
			return a >= PI && a <= 3*PI/2;
		} else {
			return a >= 3*PI/2 && a <= 2*PI;
		}
	}

	public static bool check_beam_collision(Physics q, Beam beam, double interval, out Vector.Vec2 rcp) {
		var dp = q.p.sub(beam.p);
		var x0 = dp.x;
		var y0 = dp.y;
		var r = beam.width/2 + q.r;
		var m = Math.tan(beam.a);

		// y = m*x
		// (x-x_0)^2 + (y-y_0)^2 = r^2
		// (x-x_0)^2 + (m*x-y_0)^2 - r^2 = 0
		// m^2 * x^2 - 2*m*x*y_0 - r^2 + x^2 - 2*x_0*x + x_0^2 + y_0^2 = 0
		// (m^2 + 1)*x^2 + (-2*m*y_0 - 2*x_0)*x + (x_0^2 + y_0^2 - r^2)

		var a = m*m + 1;
		var b = -2*(m*y0 + x0);
		var c = x0*x0 + y0*y0 - r*r;

		double x1, x2;
		if (solve_quadratic(a, b, c, out x1, out x2)) {
			var y1 = m*x1;
			var y2 = m*x2;
			var d1_sqr = x1*x1 + y1*y1;
			var d2_sqr = x2*x2 + y2*y2;
			var x1_valid = d1_sqr < beam.length*beam.length && check_quadrant(beam.a, x1, y1);
			var x2_valid = d2_sqr < beam.length*beam.length && check_quadrant(beam.a, x2, y2);

			if (x1_valid && (!x2_valid || d1_sqr < d2_sqr)) {
				rcp = beam.p.add(vec2(x1,m*x1));
				return true;
			} else if (x2_valid) {
				rcp = beam.p.add(vec2(x2,m*x2));
				return true;
			} else {
				return false;
			}
		} else {
			return false;
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
