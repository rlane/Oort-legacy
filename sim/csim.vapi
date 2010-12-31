[CCode (cheader_filename = "util.h")]
[CCode (cheader_filename = "ship.h")]

namespace RISC {
	[CCode (cname = "all_ships")]
	public GLib.List<Ship> all_ships;
	[CCode (cname = "trace_file")]
	public GLib.FileStream trace_file;

	namespace C {
		[CCode (cname = "envtol")]
		public int envtol(string k, int i);
		[CCode (cname = "is_win32")]
		public bool is_win32();
		[CCode (cname = "memcpy")]
		public void *memcpy(void *dest, void *src, size_t n);
		[CCode (cname = "thread_ns")]
		public uint64 thread_ns();
	}

	[CCode (cname = "rad2deg")]
	public double rad2deg(double a);
}
