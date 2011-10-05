namespace Oort {
	[CCode (cheader_filename = "glutil.h", cname = "screenshot")]
	public void screenshot(string filename);

	[CCode (cheader_filename = "glutil.h", cname = "glCheck")]
	void glCheck();
}
