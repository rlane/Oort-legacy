using Math;

[CCode (cprefix = "Vector", lower_case_cprefix = "")]
namespace Vector {

public class Mat4f {
	public float data[16];

	public float get(int row, int col) {
		return data[col*4+row];
	}

	public void set(int row, int col, float v) {
		data[col*4+row] = v;
	}

	public Mat4f.identity() {
		data = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 };
	}

	public Mat4f.scaling(float f) {
		data = {
			f, 0, 0, 0,
			0, f, 0, 0,
			0, 0, f, 0,
			0, 0, 0, 1 };
	}

	public Mat4f.translation(float x, float y, float z) {
		data = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			x, y, z, 1 };
	}

	// Copied from OpenGL SuperBible math3d.c
	public Mat4f.rotation(float angle, float x, float y, float z) {
		float mag, s, c;
		float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

		s = sinf(angle);
		c = cosf(angle);

		mag = sqrtf( x*x + y*y + z*z );

		// Identity matrix
		if (mag == 0.0f) {
			this.identity();
			return;
		}

		// Rotation matrix is normalized
		x /= mag;
		y /= mag;
		z /= mag;

		xx = x * x;
		yy = y * y;
		zz = z * z;
		xy = x * y;
		yz = y * z;
		zx = z * x;
		xs = x * s;
		ys = y * s;
		zs = z * s;
		one_c = 1.0f - c;

		this[0,0] = (one_c * xx) + c;
		this[0,1] = (one_c * xy) - zs;
		this[0,2] = (one_c * zx) + ys;
		this[0,3] = 0.0f;

		this[1,0] = (one_c * xy) + zs;
		this[1,1] = (one_c * yy) + c;
		this[1,2] = (one_c * yz) - xs;
		this[1,3] = 0.0f;

		this[2,0] = (one_c * zx) - ys;
		this[2,1] = (one_c * yz) + xs;
		this[2,2] = (one_c * zz) + c;
		this[2,3] = 0.0f;

		this[3,0] = 0.0f;
		this[3,1] = 0.0f;
		this[3,2] = 0.0f;
		this[3,3] = 1.0f;
	}

	public Mat4f.ortho(float n, float f, float r, float l, float t, float b) {
		data = { 2.0f/(r-l), 0, 0, -(r+l)/(r-l),
		         0, 2.0f/(t-b), 0, -(t+b)/(t-b),
		         0, 0, -2.0f/(f-n), -(f+n)/(f-n),
		         0, 0, 0, 1 };
	}

	public static Mat4f simpleOrtho(float x, float y, float aspect, float w)
	{
		var mt = new translation(x, y, 0);
		var proj = new ortho(-1.0f, 1.0f, w/2.0f, -w/2.0f, w*aspect/2.0f, -w*aspect/2.0f);
		return proj.multiply(mt);
	}

	public Mat4f multiply(Mat4f b) {
		var a = this;
		var c = new Mat4f();
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				float sum = 0;
				for (int k = 0; k < 4; k++) {
					sum += a[i,k]*b[k,j];
				}
				c[i,j] = sum;
			}
		}
		return c;
	}
}

}
