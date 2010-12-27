[CCode (cheader_filename = "util.h")]
[CCode (cheader_filename = "ship.h")]

namespace RISC {
	[CCode (cname = "all_ships")]
	public GLib.List<Ship> all_ships;
	[CCode (cname = "trace_file")]
	public GLib.FileStream trace_file;

	[CCode (has_target = false)]
	public delegate void OnShipCreated(Ship s);
	[CCode (cname = "gfx_ship_create_cb")]
	public OnShipCreated gfx_ship_create_cb;

	namespace C {
		[CCode (cname = "envtol")]
		public int envtol(string k, int i);
	}

	namespace CShip {
		[CCode (cname = "ship_get_energy")]
		public double get_energy(Ship s);

		[CCode (cname = "ship_create")]
		public static unowned Ship create(string filename, string class_name, RISC.Team team, Vector.Vec2 p, Vector.Vec2 v, string orders, uint32 seed);
		[CCode (cname = "ship_ai_run")]
		public static bool ai_run(Ship s, int len);

		[CCode (cname = "debug_hook")]
		public static void debug_hook(Lua.LuaVM L, ref Lua.Debug a);
	}

	[CCode (cname = "rad2deg")]
	public double rad2deg(double a);
}
