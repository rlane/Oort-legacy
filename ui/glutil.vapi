namespace RISC {
	[CCode (cheader_filename = "glutil.h", cname = "screenshot")]
	public void screenshot(string filename);

	[CCode (cheader_filename = "glutil.h", cname = "font")]
	public uint8 *font;
}
