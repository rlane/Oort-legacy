[CCode (lower_case_cprefix = "m3d", cheader_filename="math3d.h")]
namespace Math3D {
	void MatrixMultiply44(float* product, float* a, float* b);
	void LoadIdentity44(float* m);
	void ScaleMatrix44(float* m, float x, float y, float z);
	void TranslationMatrix44(float* m, float x, float y, float z);
	void RotationMatrix44(float* m, float angle, float x, float y, float z);
}
