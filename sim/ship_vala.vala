using Lua;

[CCode (free_function = "ship_destroy")]
[Compact]
public class RISC.Ship {
	public const int TAIL_SEGMENTS = 16;
	public const int TAIL_TICKS = 4;
	public const int MAX_DEBUG_LINES = 32;

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
		public DebugLine lines[32];
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
	public Vector.Vec2 tail[16];
	public int tail_head;
	public int last_shot_tick;
	public GLib.Queue mq;
	public uint64 line_start_time;
	public char line_info[256];
	public Gfx gfx;
	public Debug debug;

	[CCode (cname = "all_ships")]
	public static List<Ship> all_ships;
	[CCode (cname = "new_ships")]
	public static List<Ship> new_ships;
	[CCode (cname = "new_ships_lock")]
	public static Mutex new_ships_lock;

	public static void init() {
		new_ships_lock = new Mutex();
	}

	public static void purge() {
		unowned List<Ship> cur = all_ships;
		while (cur != null) {
			unowned List<Ship> next = cur.next;
			if (cur.data.dead) {
				all_ships.delete_link(cur);
			}
			cur = next;
		}
	}

	public static void shutdown() {
		new_ships_lock = null;
		new_ships = null;
		all_ships = null;
	}

	public static void tick(double t) {
		all_ships.concat((owned) new_ships);
		new_ships = null;

		foreach (unowned Ship s in all_ships) {
			Task.task((Task.TaskFunc)s.tick_one, s, null);
		}
		Task.wait();
	}

	public void tick_one() {
		if (Game.ticks % TAIL_TICKS == 0) {
			tail[tail_head++] = physics.p;
			if (tail_head == TAIL_SEGMENTS) tail_head = 0;
		}

		if (!ai_dead) {
			var ret = ai_run(10000);
			if (!ret) ai_dead = true;
		}

		if (!ai_dead) {
			global_lua.set_hook(CShip.debug_hook, 0, 0);
			global_lua.get_global("tick_hook");
			global_lua.call(0, 0);
		}
	}

	public bool ai_run(int len) {
		var debug_mask = Lua.EventMask.COUNT;
		if (trace_file != null) debug_mask |= Lua.EventMask.LINE;
		lua.set_hook(CShip.debug_hook, debug_mask, len);

		var result = lua.resume(0);
		if (result == ThreadStatus.YIELD) {
			return true;
		} else if (result == 0) {
			message("ship %u terminated", api_id);
			return false;
		} else {
			message("ship %u error %s", api_id, lua.to_string(-1));
			stderr.printf("backtrace:\n");
			Lua.Debug ar;
			for (int i = 0; lua.get_stack(i, out ar); i++) {
				if (!lua.get_info("nSl", out ar)) continue;
				stderr.printf("  %d: %s %s %s @ %s:%d\n", i, ar.what, ar.name_what, ar.name, ar.short_src, ar.current_line);
			}
			return false;
		}
	}

	public double get_energy() {
		return CShip.get_energy(this);
	}
}
