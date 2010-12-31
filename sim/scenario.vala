using RISC;
using Lua;
using Vector;

namespace RISC.Scenario {
	bool load(string scenario, string[] ais) {
		if (scenario.has_suffix(".json")) {
			return load_json(scenario, ais);
		} else if (scenario.has_suffix(".lua")) {
			return load_lua(scenario, ais);
		} else {
			warning("Unexpected scenario filename extension");
			return false;
		}
	}

	bool load_json(string scenario, string[] ais) {
		string data;
		try {
			FileUtils.get_contents(scenario, out data);
		} catch (FileError e) {
			warning("Failed to read scenario: %s", e.message);
			return false;
		}

		var root = cJSON.parse(data);
		if (root == null) {
			warning("Failed to parse scenario");
			return false;
		} else if (root.type != cJSON.Type.Object) {
			warning("root is not an object");
			return false;
		}

		unowned cJSON teams = root.objectItem("teams");
		if (teams == null || teams.type != cJSON.Type.Object) {
			warning("teams field is not an object");
			return false;
		}

		unowned cJSON team_obj = teams.child;
		int i = 0;
		int ai_index = 0;
		while (team_obj != null) {
			if (team_obj.type != cJSON.Type.Object) {
				warning("team definition teams.%s must be an object", team_obj.name);
				return false;
			}

			unowned cJSON color_obj = team_obj.objectItem("color");
			if (color_obj == null || color_obj.type != cJSON.Type.Object) {
				warning("teams.%s.color field is not an object", team_obj.name);
				return false;
			}

			unowned cJSON color_red = color_obj.objectItem("r");
			if (color_red == null || color_red.type != cJSON.Type.Number) {
				warning("teams.%s.color.r field is not a number", team_obj.name);
				return false;
			}
			if (color_red.int < 0 || color_red.int > 255) {
				warning("teams.%s.color.r must be in the range [0,255]", team_obj.name);
				return false;
			}

			unowned cJSON color_green = color_obj.objectItem("g");
			if (color_green == null || color_green.type != cJSON.Type.Number) {
				warning("teams.%s.color.g field is not a number", team_obj.name);
				return false;
			}
			if (color_green.int < 0 || color_green.int > 255) {
				warning("teams.%s.color.g must be in the range [0,255]", team_obj.name);
				return false;
			}

			unowned cJSON color_blue = color_obj.objectItem("b");
			if (color_blue == null || color_blue.type != cJSON.Type.Number) {
				warning("teams.%s.color.b is not a number", team_obj.name);
				return false;
			}
			if (color_blue.int < 0 || color_blue.int > 255) {
				warning("teams.%s.color.b must be in the range [0,255]", team_obj.name);
				return false;
			}

			uint32 color = color_red.int << 24 | color_green.int << 16 | color_blue.int << 8;

			unowned cJSON filename_obj = team_obj.objectItem("filename");
			if (filename_obj != null && filename_obj.type != cJSON.Type.String) {
				warning("teams.%s.color.filename must be a string", filename_obj.name);
				return false;
			}

			string filename;
			if (filename_obj != null) {
				filename = data_path(filename_obj.string);
			}	else {
				if (ai_index >= ais.length) {
					warning("too few AIs supplied");
					return false;
				}
				filename = ais[ai_index++];
			}

			uint8[] code;
			try {
				FileUtils.get_data(filename, out code);
			} catch (FileError e) {
				warning("Failed to read AI script: %s", e.message);
				return false;
			}

			Team.create(team_obj.name, filename, code, color);
			unowned Team team = Team.lookup(team_obj.name);
			assert(team != null);

			unowned cJSON ships = team_obj.objectItem("ships");
			if (ships == null || ships.type != cJSON.Type.Array) {
				warning("teams.%s.ships field is not an array", team_obj.name);
				return false;
			}

			unowned cJSON ship_obj = ships.child;
			int j = 0;
			while (ship_obj != null) {
				if (ship_obj.type != cJSON.Type.Object) {
					warning("ship definition teams.%s.ships[%d] must be an object", team_obj.name, j);
					return false;
				}

				unowned cJSON klass_name = ship_obj.objectItem("class");
				if (klass_name.type != cJSON.Type.String) {
					warning("field teams.%s.ships[%d].class must be a string", team_obj.name, j);
					return false;
				}

				unowned ShipClass klass = ShipClass.lookup(klass_name.string);
				if (klass == null) {
					warning("field teams.%s.ships[%d].class must be a valid ship class", team_obj.name, j);
					return false;
				}

				unowned cJSON x_obj = ship_obj.objectItem("x");
				if (x_obj.type != cJSON.Type.Number) {
					warning("field teams.%s.ships[%d].x must be a number", team_obj.name, j);
					return false;
				}

				unowned cJSON y_obj = ship_obj.objectItem("y");
				if (y_obj.type != cJSON.Type.Number) {
					warning("field teams.%s.ships[%d].y must be a number", team_obj.name, j);
					return false;
				}

				Ship s = new Ship(klass, team, vec2(x_obj.double, y_obj.double), vec2(0,0), Game.prng.next_int());

				if (!s.create_ai(null)) {
					warning("Failed to create AI");
					return false;
				}

				Ship.register((owned)s);
				j++;
				ship_obj = ship_obj.next;
			}

			i++;
			team_obj = team_obj.next;
		}

		if (i < 2) {
			warning("must define at least 2 teams");
			return false;
		}

		if (ai_index != ais.length) {
			warning("too many AIs supplied");
			return false;
		}

		return true;
	}

	bool load_lua(string scenario, string[] ais) {
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
			warning("Failed to load scenario: %s", L.to_string(-1));
			return false;
		}

		return true;
	}

	private int api_team(LuaVM L) {
		string name = L.check_string(1);
		string filename = L.check_string(2);
		uint32 color = L.check_long(3);
		uint8[] code;
		try {
			FileUtils.get_data(filename, out code);
		} catch (FileError e) {
			L.err(@"Failed to load AI: $(e.message)");
		}
		Team.create(name, filename, code, color);
		return 0;	
	}

	private int api_ship(LuaVM L) {
		unowned string ship_class_name = L.check_string(1);
		unowned string team_name = L.check_string(2);
		double x = L.check_number(3);
		double y = L.check_number(4);
		uint8[] orders = L.opt_data(5, "");

		unowned ShipClass klass = ShipClass.lookup(ship_class_name);
		if (klass == null) return L.arg_error(1, "invalid ship class");

		unowned Team team = Team.lookup(team_name);
		if (team == null) return L.arg_error(2, "invalid team");

		Ship s = new Ship(klass, team, vec2(x,y), vec2(0,0), Game.prng.next_int());

		if (!s.create_ai(orders)) {
			return L.err("Failed to create AI");
		}

		Ship.register((owned)s);

		return 0;
	}
}

