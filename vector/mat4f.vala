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
}

}
