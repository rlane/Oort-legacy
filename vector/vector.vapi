[CCode (cprefix = "Vector", lower_case_cprefix = "")]
namespace Vector {
	[CCode (cname = "Vec2", cheader_filename = "vector.h")]
	[SimpleType]
	public struct Vec2 {
		public double x;
		public double y;
		public double abs ();
		public Vector.Vec2 add (Vector.Vec2 v);
		public Vector.Vec2 scale (double f);
		public Vector.Vec2 sub (Vector.Vec2 v);
		public double dot (Vector.Vec2 v);
		public double distance (Vector.Vec2 v);

		public string to_string() {
			return "(%0.3g, %0.3g)".printf(x, y);
		}
	}
	public static Vector.Vec2 vec2 (double x, double y);
}
