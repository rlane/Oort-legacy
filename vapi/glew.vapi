[CCode (cheader_filename="GL/glew.h")]
namespace GLEW {
	[CCode (cname = "glewInit")]
	public bool init();

	[CCode (cname = "GLEW_ARB_window_pos")]
	public bool ARB_window_pos;

	[CCode (cname = "glWindowPos2i")]
	public void glWindowPos2i(int x, int y);
}
