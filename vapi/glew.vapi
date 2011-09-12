[CCode (cheader_filename="GL/glew.h")]
namespace GLEW {
	[CCode (cname = "glewInit")]
	public bool init();

	[CCode (cname = "GLEW_ARB_window_pos")]
	public bool ARB_window_pos;

	[CCode (cname = "glWindowPos2i")]
	public void glWindowPos2i(int x, int y);

	[CCode (cname = "glewGetErrorString")]
	public unowned string glewGetErrorString(GL.GLenum err);

	void glCheck() {
		var err = GL.glGetError();
		if (err != GL.GL_NO_ERROR) {
			var str = glewGetErrorString(err);
			GLib.error(str);
		}
	}
}
