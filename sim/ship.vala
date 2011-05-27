using Lua;
using Vector;
using Math;

[Compact]
public class Oort.Ship {
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
		public uint8[] data;

		~Msg() {
			//print("destroy msg %p\n", this);
		}
	}

	public uint32 api_id;
	public string hex_id;
	public unowned ShipClass @class;
	public unowned Team team;
	public Physics physics;
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
	public weak Game game;
	public bool controlled;

	[CCode (has_target = false)]
	public delegate void OnShipCreated(Ship s);
	public static OnShipCreated gfx_create_cb;

	public static int compare(Ship a, Ship b) {
		return (int)(a.api_id - b.api_id);
	}

	public Ship(Game game, ShipClass klass, Team team, Vec2 p, Vec2 v, double h, uint32 seed) {
		this.game = game;
		this.class = klass;
		this.team = team;
		physics = new Physics() { p=p, p0=p, v=v, acc=vec2(0,0), m=klass.reaction_mass+klass.mass, r=klass.radius, h=h, w=0, wa=0 };
		prng = new Rand.with_seed(seed);
		mq = new Queue<Msg>();
		api_id = prng.next_int();
		dead = false;
		ai_dead = false;
		controlled = false;
		hull = klass.hull;
		tail_head = 0;
		for (int i = 0; i < 16 /*XXX*/; i++) { tail[i] = vec2(double.NAN, double.NAN); }

		if (gfx_create_cb != null) {
			gfx_create_cb(this);
		}

		hex_id = "%.8x".printf(api_id);
	}

	public bool create_ai(uint8[]? orders) {
		global_lua = new LuaVM();
		mem.cur = 0;
		mem.limit = 1<<20;
		mem.allocator = global_lua.get_alloc_func(out mem.allocator_ud);
		global_lua.set_alloc_func((Lua.AllocFunc)ai_allocator, this);
		global_lua.open_libs();
		global_lua.register("sys_thrust_main", api_thrust_main);
		global_lua.register("sys_thrust_lateral", api_thrust_lateral);
		global_lua.register("sys_thrust_angular", api_thrust_angular);
		global_lua.register("sys_position", api_position);
		global_lua.register("sys_heading", api_heading);
		global_lua.register("sys_velocity", api_velocity);
		global_lua.register("sys_angular_velocity", api_angular_velocity);
		global_lua.register("sys_create_bullet", api_create_bullet);
		global_lua.register("sys_create_beam", api_create_beam);
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

		uint8 *id = (uint8*) (&api_id);
		global_lua.push_lstring(id, 4);
		global_lua.set_global("id");

		global_lua.push_string(hex_id);
		global_lua.set_global("hex_id");

		global_lua.push_string(@class.name);
		global_lua.set_global("class");

		global_lua.push_string(team.name);
		global_lua.set_global("team");

		if (orders != null) {
			global_lua.push_data(orders);
			global_lua.set_global("orders");
		}

		global_lua.push_number(game.scn.radius);
		global_lua.set_global("scenario_radius");

		if (global_lua.load_buffer(game.ships_code, "ships.lua") != 0) {
			error("Failed to load ships.lua: %s", global_lua.to_string(-1));
		}
		global_lua.call(0,0);

		if (global_lua.load_buffer(game.lib_code, "lib.lua") != 0) {
			error("Failed to load lib.lua: %s", global_lua.to_string(-1));
		}
		global_lua.set_global("lib");

		if (global_lua.load_buffer(game.strict_code, "struct.lua") != 0) {
			error("Failed to load strict.lua: %s", global_lua.to_string(-1));
		}
		global_lua.set_global("strict");

		if (global_lua.load_buffer(game.runtime_code, "runtime.lua") != 0) {
			error("Failed to load runtime.lua: %s", global_lua.to_string(-1));
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

	public void tick() {
		if (game.ticks % TAIL_TICKS == 0) {
			tail[tail_head++] = physics.p;
			if (tail_head == TAIL_SEGMENTS) tail_head = 0;
		}

		if (!ai_dead && !controlled) {
			var ret = ai_run((int)(this.class.cpu*Game.TICK_LENGTH));
			if (!ret) ai_dead = true;
		}

		if (!ai_dead) {
			global_lua.get_global("tick_hook");
			global_lua.call(0, 0);
		}

		physics.m = @class.mass + get_reaction_mass();
	}

	public static void debug_hook(Lua.LuaVM L, ref Lua.Debug a) {
		if (a.event == Lua.EventHook.COUNT) {
			L.get_global("debug_count_hook");
			L.call(0, 0);
			L.yield(0);
		} else if (a.event == Lua.EventHook.LINE) {
			unowned Ship s = lua_ship(L);
			uint64 elapsed = Util.thread_ns() - s.line_start_time;
			if (!L.get_info("nSl", ref a)) error("debug hook aborted");
			if (s.line_info != null) {
				s.game.trace_file.printf("%ld\t%s\t%s\n", (long)elapsed, s.hex_id, s.line_info);
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
		bool ret;
		var debug_mask = Lua.EventMask.COUNT;
		if (game.trace_file != null) debug_mask |= Lua.EventMask.LINE;
		lua.set_hook(debug_hook, debug_mask, len);

		var result = lua.resume(0);
		if (result == ThreadStatus.YIELD) {
			ret = true;
		} else if (result == 0) {
			message("ship %s terminated", hex_id);
			ret = false;
		} else {
			game.log_lock.lock();
			message("ship %s error %s", hex_id, lua.to_string(-1));
			stderr.printf("backtrace:\n");
			Lua.Debug ar;
			for (int i = 0; lua.get_stack(i, out ar); i++) {
				if (!lua.get_info("nSl", ref ar)) {
					stderr.printf("  %d: error", i);
				} else {
					stderr.printf("  %d: %s %s %s @ %s:%d\n", i, ar.what, ar.name_what, ar.name, ar.short_src, ar.current_line);
				}
			}
			game.log_lock.unlock();
			ret = false;
		}

		lua.set_hook(debug_hook, 0, 0);
		return ret;
	}

	public static void *RKEY = (void*)0xAABBCC02;

	public static unowned Ship lua_ship(LuaVM L) {
		L.push_lightuserdata(RKEY);
		L.get_table(Lua.PseudoIndex.REGISTRY);
		void *v = L.to_userdata(-1);
		L.pop(1);
		return (Ship)v;
	}

	public static int api_thrust_main(LuaVM L) {
		unowned Ship s = lua_ship(L);
		s.physics.acc.x = L.check_number(1);
		return 0;
	}

	public static int api_thrust_lateral(LuaVM L) {
		unowned Ship s = lua_ship(L);
		s.physics.acc.y = L.check_number(1);
		return 0;
	}

	public static int api_thrust_angular(LuaVM L) {
		unowned Ship s = lua_ship(L);
		s.physics.wa = L.check_number(1);
		return 0;
	}

	public static int api_position(LuaVM L) {
		unowned Ship s = lua_ship(L);
		L.push_number(s.physics.p.x);
		L.push_number(s.physics.p.y);
		return 2;
	}

	public static int api_heading(LuaVM L) {
		unowned Ship s = lua_ship(L);
		L.push_number(s.physics.h);
		return 1;
	}

	public static int api_velocity(LuaVM L) {
		unowned Ship s = lua_ship(L);
		L.push_number(s.physics.v.x);
		L.push_number(s.physics.v.y);
		return 2;
	}

	public static int api_angular_velocity(LuaVM L) {
		unowned Ship s = lua_ship(L);
		L.push_number(s.physics.w);
		return 1;
	}

	public static int api_create_bullet(LuaVM L) {
		unowned Ship s = lua_ship(L);

		double x = L.check_number(1);
		double y = L.check_number(2);
		double vx = L.check_number(3);
		double vy = L.check_number(4);
		double m = L.check_number(5);
		double r = L.check_number(6);
		double ttl = L.check_number(7);
		int type = L.check_int(8);

		var p = vec2(x,y);
		var v = vec2(vx,vy);
		var acc = vec2(0,0);

		var physics = new Physics() { p=p, p0=p, v=v, acc=acc, m=m, r=r };
		var b = new Bullet() { shooter_id=s.api_id, team=s.team, physics=(owned)physics, ttl=ttl, type=(BulletType)type };

		s.game.new_bullets_lock.lock();
		s.game.new_bullets.append((owned) b);
		s.game.new_bullets_lock.unlock();

		return 0;
	}

	public static int api_create_beam(LuaVM L) {
		unowned Ship s = lua_ship(L);

		double x = L.check_number(1);
		double y = L.check_number(2);
		double a = L.check_number(3);
		double length = L.check_number(4);
		double width = L.check_number(5);
		double damage = L.check_number(6);
		int graphics = L.check_int(7);

		var b = new Beam() { team=s.team,
		                     p=vec2(x,y),
		                     a=a,
		                     length=length,
		                     width=width,
		                     damage=damage,
		                     graphics=(BeamGraphics)graphics };

		s.game.new_beams_lock.lock();
		s.game.new_beams.append((owned) b);
		s.game.new_beams_lock.unlock();

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

		Ship child = new Ship(s.game, klass, s.team, s.physics.p, s.physics.v, s.physics.h, s.prng.next_int());

		if (!child.create_ai(orders)) {
			return L.err("Failed to create AI");
		}

		s.game.new_ships_lock.lock();
		s.game.new_ships.append((owned)child);
		s.game.new_ships_lock.unlock();

		return 0;
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
		uint8[] data = L.check_data(1);

		var msg = new Msg() { data=(owned)data };
		//print("publish %p\n", msg);

		s.game.radio_lock.lock();

		foreach (unowned Ship s2 in s.game.all_ships) {
			if (s == s2 || s.team != s2.team) continue;
			s2.mq.push_tail(msg);
		}

		s.game.radio_lock.unlock();
			
		return 0;	
	}

	public static int api_recv(LuaVM L) {
		unowned Ship s = lua_ship(L);

		s.game.radio_lock.lock();

		Msg msg = s.mq.pop_head();
		if (msg != null) {
			L.push_data(msg.data);
		}

		s.game.radio_lock.unlock();

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
		unowned Ship s = lua_ship(L);
		SensorQuery query = SensorQuery();
		L.check_type(1, Lua.Type.TABLE);
		query.parse(L, 1);

		L.push_lightuserdata((void*)SensorContact.MAGIC);
		L.raw_get(Lua.PseudoIndex.REGISTRY);
		int metatable_index = L.get_top();

		L.create_table((int)s.game.all_ships.length(), 0);
		int i = 0;
		foreach (unowned Ship s2 in s.game.all_ships) {
			if (query.enabled(QueryOption.LIMIT) && i >= query.limit) break;
			if (query.match(s2)) {
				i++;
				SensorContact.create(L, s2, metatable_index);
				L.raw_seti(-2, i);
			}
		}

		return 1;
	}

	public static int api_sensor_contact(LuaVM L) {
		unowned Ship s = lua_ship(L);
		size_t n;
		uint8 *id = L.check_lstring(1, out n);
		if (n != 4) L.err("invalid contact id");
		L.pop(1);

		uint32 _id = *((uint32*)id);
		foreach (unowned Ship s2 in s.game.all_ships) {
			if (s2.api_id == _id) {
				L.push_lightuserdata((void*)SensorContact.MAGIC);
				L.raw_get(Lua.PseudoIndex.REGISTRY);
				SensorContact.create(L, s2, L.get_top());
				return 1;
			}
		}

		return 0;
	}

	public void refuel_hit(double m) {
		global_lua.get_global("refuel_hit");
		global_lua.push_number(m);
		global_lua.call(1, 0);
	}

	public double get_energy() {
		global_lua.get_global("energy");
		global_lua.call(0, 1);
		double e = global_lua.to_number(-1);
		global_lua.pop(1);
		return e;
	}

	public double get_reaction_mass() {
		global_lua.get_global("reaction_mass");
		global_lua.call(0, 1);
		double e = global_lua.to_number(-1);
		global_lua.pop(1);
		return e;
	}

	~Ship() {
		while (mq.pop_head() != null);
		//print("destroy ship %p\n", this);
	}

	public void control_begin() {
		controlled = true;
		global_lua.get_global("control_begin");
		global_lua.call(0, 0);
	}

	public void control_end() {
		controlled = false;
		global_lua.get_global("control_end");
		global_lua.call(0, 0);
	}

	public void control(string key, bool pressed) {
		global_lua.get_global("control");
		global_lua.push_string(key);
		global_lua.push_boolean(pressed);
		global_lua.call(2, 0);
	}
}
