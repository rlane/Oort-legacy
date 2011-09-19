[CCode (cprefix = "Vector", lower_case_cprefix = "", cprefix = "")]
namespace Vector {
	[CCode (cname = "Vec2", cheader_filename = "vec2d.h")]
	[SimpleType]
	public struct Vec2 {
		public double x;
		public double y;
		public double *data;
		public Vector.Vec2f to_vec2f();
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

	[CCode (cname = "Vec2f", cheader_filename = "vec2f.h")]
	[SimpleType]
	public struct Vec2f {
		public float x;
		public float y;
		public float *data;
		public Vector.Vec4f projectXY(ref Mat4f mModelView, ref Mat4f mProjection, [CCode (array_length=false)] int viewport[4]);

		public string to_string() {
			return "(%0.3g, %0.3g)".printf(x, y);
		}
	}
	[CCode (cname = "vec2f", cheader_filename = "vec2f.h")]
	public static Vector.Vec2f vec2f (float x, float y);

	[CCode (cname = "Vec4f", cheader_filename = "vec4f.h")]
	[SimpleType]
	public struct Vec4f {
		public float x;
		public float y;
		public float z;
		public float w;
		public float *data;
		public float abs ();
		public Vector.Vec4f add (Vector.Vec4f v);
		public Vector.Vec4f scale (float f);
		public Vector.Vec4f sub (Vector.Vec4f v);
		public float dot (Vector.Vec4f v);
		public float distance (Vector.Vec4f v);
		public Vector.Vec4f transform(ref Mat4f m);
		public Vector.Vec4f projectXY(ref Mat4f mModelView, ref Mat4f mProjection, [CCode (array_length=false)] int viewport[4]);

		public string to_string() {
			return "(%0.3g, %0.3g, %0.3g, %0.3g)".printf(x, y, z, w);
		}
	}
	[CCode (cname = "vec4f", cheader_filename = "vec4f.h")]
	public static Vector.Vec4f vec4f (float x, float y, float z, float w);

	[CCode (cheader_filename = "mat4f.h")]
	public struct Mat4f {
		public float data[];

		public float get(int row, int col) {
			return data[col*4+row];
		}

		public void set(int row, int col, float v) {
			data[col*4+row] = v;
		}

		public static void load_identity(out Mat4f m);
		public static void load_scale(out Mat4f m, float x, float y, float z);
		public static void load_translation(out Mat4f m, float x, float y, float z);
		public static void load_rotation(out Mat4f m, float angle, float x, float y, float z);
		public static void load_ortho(out Mat4f m, float n, float f, float r, float l, float t, float b);
		public static void load_simple_ortho(out Mat4f m, float x, float y, float aspect, float w);

		public static void multiply(out Mat4f dest, ref Mat4f a, ref Mat4f b);
		public static void invert(out Mat4f dest, ref Mat4f a);
	}
}
