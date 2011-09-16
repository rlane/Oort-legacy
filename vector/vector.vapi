[CCode (cprefix = "Vector", lower_case_cprefix = "")]
namespace Vector {
	[CCode (cname = "Vec2", cheader_filename = "vec2d.h")]
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
		public Vector.Vec2 rotate (double angle);

		public string to_string() {
			return "(%0.3g, %0.3g)".printf(x, y);
		}
	}
	[CCode (cname = "vec2", cheader_filename = "vec2d.h")]
	public static Vector.Vec2 vec2 (double x, double y);

	[CCode (cname = "Vec4f", cheader_filename = "vec4f.h")]
	[SimpleType]
	public struct Vec4f {
		public float x;
		public float y;
		public float z;
		public float w;
		public float abs ();
		public Vector.Vec4f add (Vector.Vec4f v);
		public Vector.Vec4f scale (float f);
		public Vector.Vec4f sub (Vector.Vec4f v);
		public float dot (Vector.Vec4f v);
		public float distance (Vector.Vec4f v);

		public string to_string() {
			return "(%0.3g, %0.3g, %0.3g, %0.3g)".printf(x, y, z, w);
		}
	}
	[CCode (cname = "vec4f", cheader_filename = "vec4f.h")]
	public static Vector.Vec4f vec4f (float x, float y, float z, float w);

	[CCode (cheader_filename = "vector_gen.h")]
	public class Mat4f {
		public float data[];

		public Mat4f.identity();
		public Mat4f.scaling(float f);
		public Mat4f.translation(float x, float y, float z);
		public Mat4f.rotation(float angle, float x, float y, float z);
	}
}
