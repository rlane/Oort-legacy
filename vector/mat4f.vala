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
		Math3D.LoadIdentity44(data);
	}

	public Mat4f.scale(float x, float y, float z) {
		Math3D.ScaleMatrix44(data, x, y, z);
	}

	public Mat4f.translation(float x, float y, float z) {
		Math3D.TranslationMatrix44(data, x, y, z);
	}

	public Mat4f.rotation(float angle, float x, float y, float z) {
		Math3D.RotationMatrix44(data, angle, x, y, z);
	}

	public Mat4f.ortho(float n, float f, float r, float l, float t, float b) {
		data = { 2.0f/(r-l), 0, 0, 0,
		         0, 2.0f/(t-b), 0, 0,
		         0, 0, -2.0f/(f-n), 0,
		         -(r+l)/(r-l), -(t+b)/(t-b), -(f+n)/(f-n), 1 };
	}

	public static Mat4f simpleOrtho(float x, float y, float aspect, float w)
	{
		return new ortho(-1.0f, 1.0f, x+w/2.0f, x-w/2.0f, y+w*aspect/2.0f, y-w*aspect/2.0f);
	}

	public Mat4f multiply(Mat4f b) {
		var c = new Mat4f();
		Math3D.MatrixMultiply44(c.data, this.data, b.data);
		return c;
	}
}

}
