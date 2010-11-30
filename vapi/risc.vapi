[CCode (cheader_filename = "renderer.h")]
[CCode (cheader_filename = "game.h")]
[CCode (cheader_filename = "particle.h")]
[CCode (cheader_filename = "glutil.h")]
[CCode (cheader_filename = "util.h")]

namespace RISC {
    [CCode (cname = "init_gl13")]
    public void init_gl13();
    [CCode (cname = "reset_gl13")]
    public void reset_gl13();
    [CCode (cname = "render_gl13")]
    public void render_gl13(bool paused);
    [CCode (cname = "reshape_gl13")]
    public void reshape_gl13(int x, int y);

		[CCode (cname = "zoom")]
		public int zoom(int x, int y, double force);
		[CCode (cname = "pick")]
		public void pick(int x, int y);
    [CCode (cname = "emit_particles")]
    public void emit_particles();

		[CCode (cname = "game_init")]
		public int game_init(int seed, string scenario, string[] ais);
		[CCode (cname = "game_purge")]
		public void game_purge();
		[CCode (cname = "game_tick")]
		public void game_tick(double tick_length);
		[CCode (cname = "game_shutdown")]
		public void game_shutdown();

		[CCode (cname = "particle_tick")]
		public void particle_tick();

		[CCode (cname = "screenshot")]
		public void screenshot(string filename);
		[CCode (cname = "glPrintf")]
		public void glPrintf(int x, int y, string fmt, ...);
		[CCode (cname = "glColor32")]
		void glColor32(uint32 c);
		
		[CCode (cname = "find_data_dir")]
		public bool find_data_dir();
		[CCode (cname = "data_path")]
		public string data_path(string subpath);
}
