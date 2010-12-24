[CCode (cheader_filename = "particle.h")]
[CCode (cheader_filename = "glutil.h")]
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

	namespace GL13 {
		[CCode (cname = "init_gl13")]
		public void init();
	}

	namespace C {
		[CCode (cname = "envtol")]
		public int envtol(string k, int i);

		[CCode (cname = "font")]
		public uint8 *font;
	}

	[CCode (cname = "screenshot")]
	public void screenshot(string filename);
	
	[CCode (cname = "data_dir")]
	public string data_dir;
	[CCode (cname = "find_data_dir")]
	public bool find_data_dir();
	[CCode (cname = "data_path")]
	public string data_path(string subpath);

	public enum ParticleType {
		[CCode (cname = "PARTICLE_HIT")]
		HIT,
		[CCode (cname = "PARTICLE_PLASMA")]
		PLASMA,
		[CCode (cname = "PARTICLE_ENGINE")]
		ENGINE,
	}

	[CCode (cname = "struct particle", destroy_function = "")]
	[Compact]
	public class Particle {
		public Vector.Vec2 p;
		public Vector.Vec2 v;
		public uint16 ticks_left;
		public ParticleType type;

		[CCode (cname = "MAX_PARTICLES")]
		public static int MAX;

		[CCode (cname = "particles")]
		public static Particle array[];

		[CCode (cname = "particle_get")]
		public static unowned Particle get(int i);

		[CCode (cname = "particle_create")]
		public static void create(ParticleType type, Vector.Vec2 p, Vector.Vec2 v, uint16 lifetime);
		[CCode (cname = "particle_shower")]
		public static void shower(ParticleType type,
		                          Vector.Vec2 p0, Vector.Vec2 v0, Vector.Vec2 v,
		                          double s_max, uint16 life_min, uint16 life_max,
		                          uint16 count);
		[CCode (cname = "particle_tick")]
		public static void tick();
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
