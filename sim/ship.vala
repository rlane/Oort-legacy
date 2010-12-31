using Lua;
using Vector;
using Math;

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

	public class Msg {
		public size_t len;
		public uint8 *data;

		~Msg() {
			//print("destroy msg %p\n", this);
			free(data);
		}
	}

	public uint32 api_id;
	public unowned ShipClass @class;
	public unowned Team team;
	public Physics physics;
	public double energy;
	public double hull;
	public weak Lua.LuaVM lua;
	public Lua.LuaVM global_lua;
	public LuaMemState mem;
	public GLib.Rand prng;
	public bool dead;
	public bool ai_dead;
	public Vector.Vec2 tail[16];
	public int tail_head;
	public int last_shot_tick;
	public Queue<Msg> mq;
	public uint64 line_start_time;
	public string line_info;
	public Gfx gfx;
	public Debug debug;

	public static List<Ship> all_ships;
	public static List<Ship> new_ships;
	public static Mutex new_ships_lock;
	public static Mutex radio_lock;
	public static GLib.FileStream trace_file;

	[CCode (has_target = false)]
	public delegate void OnShipCreated(Ship s);
	public static OnShipCreated gfx_create_cb;

	public static void init() {
		new_ships_lock = new Mutex();
		radio_lock = new Mutex();
	}

	[CCode (cname = "leak")]
	static extern Ship unleak_ship(Ship s);

	public static void purge() {
		unowned List<Ship> cur = all_ships;
		while (cur != null) {
			unowned List<Ship> next = cur.next;
			if (cur.data.dead) {
				unleak_ship(cur.data);
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

	public Ship(ShipClass klass, Team team, Vec2 p, Vec2 v, uint32 seed) {
		this.team = team;
		@class = klass;
		physics = Physics.create(p, p, v, vec2(0,0), 0, 0, 1, klass.radius);
		prng = new Rand.with_seed(seed);
		mq = new Queue<Msg>();
		api_id = prng.next_int();
		dead = false;
		ai_dead = false;
		hull = klass.hull;
		tail_head = 0;
		for (int i = 0; i < 16 /*XXX*/; i++) { tail[i] = vec2(double.NAN, double.NAN); }

		if (gfx_create_cb != null) {
			gfx_create_cb(this);
		}
	}

	public bool create_ai(uint8[] orders) {
		global_lua = new LuaVM();
		mem.cur = 0;
		mem.limit = 1<<20;
		mem.allocator = global_lua.get_alloc_func(out mem.allocator_ud);
		global_lua.set_alloc_func((Lua.AllocFunc)ai_allocator, this);
		global_lua.open_libs();
		global_lua.register("sys_thrust", api_thrust);
		global_lua.register("sys_yield", api_yield);
		global_lua.register("sys_position", api_position);
		global_lua.register("sys_velocity", api_velocity);
		global_lua.register("sys_create_bullet", api_create_bullet);
		global_lua.register("sys_sensor_contacts", api_sensor_contacts);
		global_lua.register("sys_sensor_contact", api_sensor_contact);
		global_lua.register("sys_random", api_random);
		global_lua.register("sys_send", api_send);
		global_lua.register("sys_recv", api_recv);
		global_lua.register("sys_spawn", api_spawn);
		global_lua.register("sys_die", api_die);
		global_lua.register("sys_debug_line", api_debug_line);
		global_lua.register("sys_clear_debug_lines", api_clear_debug_lines);

		global_lua.push_lightuserdata(RKEY);
		global_lua.push_lightuserdata(this);
		global_lua.set_table(Lua.PseudoIndex.REGISTRY);

		SensorContact.register(global_lua);

		global_lua.push_string(@class.name);
		global_lua.set_global("class");

		global_lua.push_string(team.name);
		global_lua.set_global("team");

		global_lua.push_data(orders);
		global_lua.set_global("orders");

		string data_dir = Paths.resource_dir.get_path();
		global_lua.push_string(data_dir);
		global_lua.set_global("data_dir");

		if (global_lua.load_buffer(Game.runtime_code) != 0) {
			warning("Failed to load runtime: %s", global_lua.to_string(-1));
			return false;
		}

		global_lua.call(0,0);

		lua = global_lua.new_thread();

		lua.get_global("sandbox");

		if (lua.load_buffer(team.code, team.filename) != 0) {
			warning("Couldn't load AI: %s", lua.to_string(-1));
			return false;
		}

		lua.call(1, 1);

		return true;
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
			global_lua.set_hook(debug_hook, 0, 0);
			global_lua.get_global("tick_hook");
			global_lua.call(0, 0);
		}
	}

	public static void debug_hook(Lua.LuaVM L, ref Lua.Debug a) {
		if (a.event == Lua.EventHook.COUNT) {
			L.get_global("debug_count_hook");
			L.call(0, 0);
			L.yield(0);
		} else if (a.event == Lua.EventHook.LINE) {
			unowned Ship s = lua_ship(L);
			uint64 elapsed = Util.thread_ns() - s.line_start_time;
			if (!L.get_info("nSl", out a)) error("debug hook aborted");
			if (s.line_info != null) {
				trace_file.printf("%ld\t%u\t%s\n", (long)elapsed, s.api_id, s.line_info);
			}
			s.line_info = "%s\t%s:%d".printf(a.name, a.short_src, a.current_line);
			s.line_start_time = Util.thread_ns();
		}
	}

	public void *ai_allocator(void *ptr, size_t osize, size_t nsize) {
		mem.cur += (int)(nsize - osize);
		if (nsize > osize && mem.cur > mem.limit) return null;
		return mem.allocator(mem.allocator_ud, ptr, osize, nsize);
	}

	public bool ai_run(int len) {
		var debug_mask = Lua.EventMask.COUNT;
		if (trace_file != null) debug_mask |= Lua.EventMask.LINE;
		lua.set_hook(debug_hook, debug_mask, len);

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

	public static void *RKEY = (void*)0xAABBCC02;

	public static unowned Ship lua_ship(LuaVM L) {
		L.push_lightuserdata(RKEY);
		L.get_table(Lua.PseudoIndex.REGISTRY);
		void *v = L.to_userdata(-1);
		L.pop(1);
		return (Ship)v;
	}

	public static int api_thrust(LuaVM L) {
		unowned Ship s = lua_ship(L);
		double a = L.check_number(1);
		double f = L.check_number(2);
		s.physics.thrust = vec2(cos(a), sin(a)).scale(f * s.physics.m);
		return 0;
	}

	public static int api_yield(LuaVM L) {
		return L.yield(0);
	}

	public static int api_position(LuaVM L) {
		unowned Ship s = lua_ship(L);
		L.push_number(s.physics.p.x);
		L.push_number(s.physics.p.y);
		return 2;
	}

	public static int api_velocity(LuaVM L) {
		unowned Ship s = lua_ship(L);
		L.push_number(s.physics.v.x);
		L.push_number(s.physics.v.y);
		return 2;
	}

	public static int api_create_bullet(LuaVM L) {
		unowned Ship s = lua_ship(L);

		double x = L.check_number(1);
		double y = L.check_number(2);
		double vx = L.check_number(3);
		double vy = L.check_number(4);
		double m = L.check_number(5);
		double ttl = L.check_number(6);
		int type = L.check_int(7);

		Bullet.create(s.team, vec2(x,y), vec2(vx,vy), 1.0/32, m, ttl, (BulletType)type);

		return 0;
	}

	public static int api_random(LuaVM L) {
		unowned Ship s = lua_ship(L);
		int n = L.get_top();

		if (n == 0) {
			L.set_top(0);
			L.push_number(s.prng.next_double());
		} else if (n == 1 || n == 2) {
			int32 begin, end;
			if (n == 1) {
				begin = 1;
				end = L.check_long(1);
			} else {
				begin = L.check_long(1);
				end = L.check_long(2);
			}
			L.set_top(0);
			if (begin < end) {
				L.push_number(s.prng.int_range(begin, end));
			} else if (begin == end) {
				L.push_number(begin);
			} else {
				return L.err("end must be >= begin");
			}
		} else {
			return L.err("too many arguments");
		}
		
		return 1;
	}

	public static int api_spawn(LuaVM L) {
		unowned Ship s = lua_ship(L);
		unowned string class_name = L.check_string(1);
		unowned ShipClass klass = ShipClass.lookup(class_name);
		if (klass == null) return L.arg_error(1, "invalid ship class");
		unowned uint8[] orders = L.check_data(2);

		Ship child = new Ship(klass, s.team, s.physics.p, s.physics.v, s.prng.next_int());

		if (!child.create_ai(orders)) {
			return L.err("Failed to create AI");
		}

		Ship.register((owned)child);

		return 0;
	}

	public static void register(owned Ship s) {
		new_ships_lock.lock();
		new_ships.append((owned)s);
		new_ships_lock.unlock();
	}

	public static int api_die(LuaVM L) {
		unowned Ship s = lua_ship(L);
		s.dead = true;
		return L.yield(0);
	}	

	public static int api_debug_line(LuaVM L) {
		unowned Ship s = lua_ship(L);
		if (s.debug.num_lines == s.debug.lines.length) {
			return 0;
		}
		int i = s.debug.num_lines++;
		double x1 = L.check_number(1);
		double y1 = L.check_number(2);
		double x2 = L.check_number(3);
		double y2 = L.check_number(4);
		s.debug.lines[i].a = vec2(x1,y1);
		s.debug.lines[i].b = vec2(x2,y2);
		return 0;
	}

	public static int api_clear_debug_lines(LuaVM L) {
		unowned Ship s = lua_ship(L);
		s.debug.num_lines = 0;
		return 0;
	}

	public static int api_send(LuaVM L) {
		unowned Ship s = lua_ship(L);
		size_t len;
		uint8 *ldata = L.check_lstring(1, out len);

		uint8 *data = malloc(len);
		Util.memcpy(data, ldata, len);

		var msg = new Msg() { len=len, data=data };
		//print("publish %p\n", msg);

		radio_lock.lock();

		foreach (unowned Ship s2 in all_ships) {
			if (s == s2 || s.team != s2.team) continue;
			s2.mq.push_tail(msg);
		}

		radio_lock.unlock();
			
		return 0;	
	}

	public static int api_recv(LuaVM L) {
		unowned Ship s = lua_ship(L);

		radio_lock.lock();

		Msg msg = s.mq.pop_head();
		if (msg != null) {
			L.push_lstring(msg.data, msg.len);
		}

		radio_lock.unlock();

		return (msg != null) ? 1 : 0;
	}

	[Compact]
	public struct SensorContact {
		public uint32 magic;
		public uint32 id;
		public unowned Team team;
		public unowned ShipClass @class;
		public Vec2 p;
		public Vec2 v;

		public static const uint32 MAGIC = 0xAABBCC01u;

		public static void create(LuaVM L, Ship s, int metatable_index) {
			SensorContact *c = L.new_userdata(sizeof(SensorContact));
			c->magic = MAGIC;
			c->id = s.api_id;
			c->team = s.team;
			c->class = s.class;
			c->p = s.physics.p;
			c->v = s.physics.v;
			L.push_value(metatable_index);
			L.set_metatable(-2);
		}

		public static SensorContact *cast(LuaVM L, int index) {
			SensorContact *c = L.to_userdata(index);
			if (c != null && c->magic != MAGIC) c = null;
			if (c == null) L.arg_error(1, "sensor contact expected");
			return c;
		}

		public static int api_id(LuaVM L) {
			var c = cast(L, 1);
			L.push_lstring((uint8*)(&c->id), 4);
			return 1;
		}

		public static int api_team(LuaVM L) {
			var c = cast(L, 1);
			L.push_string(c->team.name);
			return 1;
		}

		public static int api_class(LuaVM L) {
			var c = cast(L, 1);
			L.push_string(c->class.name);
			return 1;
		}

		public static int api_position(LuaVM L) {
			var c = cast(L, 1);
			L.push_number(c->p.x);
			L.push_number(c->p.y);
			return 2;
		}

		public static int api_velocity(LuaVM L) {
			var c = cast(L, 1);
			L.push_number(c->v.x);
			L.push_number(c->v.y);
			return 2;
		}

		public static void register(LuaVM L) {
			L.push_lightuserdata((void*)MAGIC);
			L.create_table(0, 1);

			L.push_string("__index");
			L.create_table(0, 5);

			L.push_string("id");
			L.push_cfunction(api_id);
			L.set_table(-3);

			L.push_string("team");
			L.push_cfunction(api_team);
			L.set_table(-3);

			L.push_string("class");
			L.push_cfunction(api_class);
			L.set_table(-3);

			L.push_string("position");
			L.push_cfunction(api_position);
			L.set_table(-3);

			L.push_string("velocity");
			L.push_cfunction(api_velocity);
			L.set_table(-3);

			L.set_table(-3);
			L.set_table(Lua.PseudoIndex.REGISTRY);
		}
	}

	[Flags]
	public enum QueryOption {
		TEAM, ENEMY, CLASS,
		DISTANCE_GT, DISTANCE_LT,
		HULL_GT, HULL_LT,
		LIMIT,
	}

	[Compact]
	public struct SensorQuery {
		public unowned Team my_team;
		public bool enemy;
		public unowned ShipClass class;
		public double distance_gt;
		public double distance_lt;
		public double hull_gt;
		public double hull_lt;
		public uint32 limit;
		public Vec2 origin;
		public QueryOption options;

		public double option_double(LuaVM L, int idx, string key, QueryOption option) {
			double ret = double.NAN;
			L.push_string(key);
			L.raw_get(idx);
			if (L.is_number(-1)) {
				options |= option;
				ret = L.to_number(-1);
			}
			L.pop(1);
			return ret;
		}

		public unowned string? option_string(LuaVM L, int idx, string key, QueryOption option) {
			unowned string? ret = null;
			L.push_string(key);
			L.raw_get(idx);
			if (L.is_string(-1)) {
				options |= option;
				ret = L.to_string(-1);
			}
			L.pop(1);
			return ret;
		}

		public int option_int(LuaVM L, int idx, string key, QueryOption option) {
			int ret = -1;
			L.push_string(key);
			L.raw_get(idx);
			if (L.is_number(-1)) {
				options |= option;
				ret = L.to_integer(-1);
			}
			L.pop(1);
			return ret;
		}

		public bool option_boolean(LuaVM L, int idx, string key, QueryOption option) {
			bool ret = false;
			L.push_string(key);
			L.raw_get(idx);
			if (L.is_boolean(-1)) {
				options |= option;
				ret = L.to_boolean(-1);
			}
			L.pop(1);
			return ret;
		}

		public void parse(LuaVM L, int idx) {
			unowned Ship s = lua_ship(L);

			my_team = s.team;
			origin = s.physics.p;
			options = 0;

			enemy = option_boolean(L, idx, "enemy", QueryOption.ENEMY);
			var class_name = option_string(L, idx, "class", QueryOption.CLASS);
			distance_lt = option_double(L, idx, "distance_lt", QueryOption.DISTANCE_LT);
			distance_gt = option_double(L, idx, "distance_gt", QueryOption.DISTANCE_GT);
			hull_lt = option_double(L, idx, "hull_lt", QueryOption.HULL_LT);
			hull_gt = option_double(L, idx, "hull_gt", QueryOption.HULL_GT);
			limit = option_int(L, idx, "limit", QueryOption.LIMIT);

			if ((options & QueryOption.CLASS) != 0) {
				this.class = ShipClass.lookup(class_name);
				if (this.class == null) {
					L.err("invalid ship class in query");
				}
			}
		}

		public bool enabled(QueryOption o) {
			return (options & o) == o;
		}

		public bool match(Ship s) {
			if (enabled(QueryOption.ENEMY)) {
				if (enemy && my_team == s.team) return false;
				if (!enemy && my_team != s.team) return false;
			}

			if (enabled(QueryOption.CLASS) && @class != s.class) return false;
			if (enabled(QueryOption.HULL_LT) && hull_lt <= s.hull) return false;
			if (enabled(QueryOption.HULL_GT) && hull_gt >= s.hull) return false;

			if (enabled(QueryOption.DISTANCE_LT) || enabled(QueryOption.DISTANCE_GT)) {
				double distance = origin.distance(s.physics.p);
				if (enabled(QueryOption.DISTANCE_LT) && distance_lt <= distance) return false;
				if (enabled(QueryOption.DISTANCE_GT) && distance_gt >= distance) return false;
			}

			return true;
		}
	}

	public static int api_sensor_contacts(LuaVM L) {
		SensorQuery query = SensorQuery();
		L.check_type(1, Lua.Type.TABLE);
		query.parse(L, 1);

		L.push_lightuserdata((void*)SensorContact.MAGIC);
		L.raw_get(Lua.PseudoIndex.REGISTRY);
		int metatable_index = L.get_top();

		L.create_table((int)all_ships.length(), 0);
		int i = 0;
		foreach (unowned Ship s in all_ships) {
			if (query.enabled(QueryOption.LIMIT) && i >= query.limit) break;
			if (query.match(s)) {
				i++;
				SensorContact.create(L, s, metatable_index);
				L.raw_seti(-2, i);
			}
		}

		return 1;
	}

	public static int api_sensor_contact(LuaVM L) {
		size_t n;
		uint8 *id = L.check_lstring(1, out n);
		if (n != 4) L.err("invalid contact id");
		L.pop(1);

		uint32 _id = *((uint32*)id);
		foreach (unowned Ship s in all_ships) {
			if (s.api_id == _id) {
				L.push_lightuserdata((void*)SensorContact.MAGIC);
				L.raw_get(Lua.PseudoIndex.REGISTRY);
				SensorContact.create(L, s, L.get_top());
				return 1;
			}
		}

		return 0;
	}

	public double get_energy() {
		global_lua.get_global("energy");
		global_lua.call(0, 1);
		double e = global_lua.to_number(-1);
		global_lua.pop(1);
		return e;
	}

	~Ship() {
		while (mq.pop_head() != null);
		//print("destroy ship %p\n", this);
	}
}
