[CCode (cheader_filename = "particle.h")]
[CCode (cheader_filename = "glutil.h")]
[CCode (cheader_filename = "util.h")]
[CCode (cheader_filename = "ship.h")]

namespace RISC {
	[CCode (cname = "all_ships")]
	public GLib.List<Ship> all_ships;

	[CCode (has_target = false)]
	public delegate void OnShipCreated(Ship s);
	[CCode (cname = "gfx_ship_create_cb")]
	public OnShipCreated gfx_ship_create_cb;

	namespace GL13 {
		[CCode (cname = "init_gl13")]
		public void init();
	}

	namespace C {
		[CCode (cname = "glutil_vprintf")]
		public void vprintf(int x, int y, string fmt, va_list ap);

		[CCode (cname = "envtol")]
		public int envtol(string k, int i);
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

	[CCode (cname = "struct ship", destroy_function = "")]
	[Compact]
	public class Ship {
		[CCode (cname = "TAIL_SEGMENTS")]
		public const int TAIL_SEGMENTS;
		[CCode (cname = "TAIL_TICKS")]
		public const int TAIL_TICKS;
		[CCode (cname = "MAX_DEBUG_LINES")]
		public const int MAX_DEBUG_LINES;

		public struct LuaMemState {
			public Lua.AllocFunc allocator;
			public void *allocator_ud;
			public int cur;
			public int limit;
		}

		public struct Gfx {
			public void *@class;
			public double angle;
		}

		public struct DebugLine {
			public Vector.Vec2 a;
			public Vector.Vec2 b;
		}

		public struct Debug {
			public int num_lines;
			public DebugLine lines[];
		}

		public uint32 api_id;
		public unowned ShipClass @class;
		public unowned Team team;
		public Physics physics;
		public double energy;
		public double hull;
		public Lua.LuaVM lua;
		public Lua.LuaVM global_lua;
		public LuaMemState mem;
		public GLib.Rand prng;
		public bool dead;
		public bool ai_dead;
		public Vector.Vec2 tail[];
		public int tail_head;
		public int last_shot_tick;
		public GLib.Queue mq;
		public uint64 line_start_time;
		public char line_info[256];
		public Gfx gfx;
		public Debug debug;

		[CCode (cname = "ship_get_energy")]
		public double get_energy();

		[CCode (cname = "ship_create")]
		public static unowned Ship create(string filename, string class_name, RISC.Team team, Vector.Vec2 p, Vector.Vec2 v, string orders, uint32 seed);
		[CCode (cname = "ship_purge")]
		public static void purge();
		[CCode (cname = "ship_shutdown")]
		public static void shutdown();
		[CCode (cname = "ship_tick")]
		public static void tick(double tick_length);
	}

	[CCode (cname = "rad2deg")]
	public double rad2deg(double a);
}
