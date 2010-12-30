using RISC;
using Lua;
using Vector;

namespace RISC.Scenario {
	public bool load(string scenario, string[] ais) {
		var L = new LuaVM();
		L.open_libs();

		L.register("team", api_team);
		L.register("ship", api_ship);

		L.push_string(Paths.resource_dir.get_path());
		L.set_global("data_dir");

		L.push_number(ais.length);
		L.set_global("N");

		L.new_table();
		for (int i = 0; i < ais.length; i++) {
			L.push_number(i);
			L.push_string(ais[i]);
			L.set_table(-3);
		}
		L.set_global("AI");

		if (L.do_file(scenario)) {
			warning("Failed to load scenario %s: %s", scenario, L.to_string(-1));
			return false;
		}

		return true;
	}

	private int api_team(LuaVM L) {
		string name = L.check_string(1);
		string filename = L.check_string(2);
		uint32 color = L.check_long(3);
		Team.create(name, filename, color);
		return 0;	
	}

	private int api_ship(LuaVM L) {
		unowned string ship_class_name = L.check_string(1);
		unowned string team_name = L.check_string(2);
		double x = L.check_number(3);
		double y = L.check_number(4);
		size_t orders_len = 0;
		uint8 *orders = L.opt_lstring(5, "", out orders_len);

		unowned ShipClass klass = ShipClass.lookup(ship_class_name);
		if (klass == null) return L.arg_error(1, "invalid ship class");

		unowned Team team = Team.lookup(team_name);
		if (team == null) return L.arg_error(2, "invalid team");

		Ship s = new Ship(klass, team, vec2(x,y), vec2(0,0), Game.prng.next_int());

		if (s.create_ai(orders, orders_len) != 0) {
			return L.err("Failed to create AI");
		}

		Ship.register((owned)s);

		return 0;
	}
}

