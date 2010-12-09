[CCode (cprefix = "Vector", lower_case_cprefix = "")]
namespace Vector {
	[CCode (cname = "Vec2", cheader_filename = "vector_vala.h")]
	[SimpleType]
	public struct Vec2 {
		public double x;
		public double y;
		public double abs ();
		public Vector.Vec2 add (Vector.Vec2 v);
		public Vector.Vec2 scale (double f);
		public Vector.Vec2 sub (Vector.Vec2 v);
	}
	public static Vector.Vec2 vec2 (double x, double y);
}
