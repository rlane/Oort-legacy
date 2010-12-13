using RISC;
using Lua;
using Vector;

namespace RISC.Scenario {
	public bool load(string scenario, string[] ais) {
		var L = new LuaVM();
		L.open_libs();

		L.register("team", api_team);
		L.register("ship", api_ship);

		L.push_string(data_dir);
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
			error("Failed to load scenario %s: %s\n", scenario, L.to_string(-1));
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
		string ship_class_name = L.check_string(1);
		string team_name = L.check_string(2);
		double x = L.check_number(3);
		double y = L.check_number(4);
		string orders = L.opt_string(5, "");
		unowned Team team = Team.lookup(team_name);
		if (team == null) return L.arg_error(2, "invalid team");
		unowned Ship s = Ship.create(team.filename, ship_class_name, team, vec2(x,y), vec2(0,0), orders, Game.prng.next_int());
		if (s == null) return L.err("ship creation failed");
		return 0;
	}
}

