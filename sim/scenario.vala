using RISC;
using Lua;
using Vector;

namespace RISC.Scenario {
	public class ParsedScenario {
		public string name;
		public string description;
		public int num_user_ai;
		public List<ParsedTeam> teams;
	}

	public class ParsedTeam {
		public string name;
		public string filename;
		public uint32 color;
		public List<ParsedShip> ships;
	}

	public class ParsedShip {
		public unowned ShipClass @class;
		public Vec2 p;
	}

	bool load(string filename, string[] ais) {
		if (filename.has_suffix(".lua")) {
			return load_lua(filename, ais);
		}

		var scn = parse(filename);
		if (scn == null) return false;

		if (scn.num_user_ai != ais.length) {
			warning("Expected %d user AIs, got %d", scn.num_user_ai, ais.length);
			return false;
		}

		int ai_index = 0;
		foreach (ParsedTeam pteam in scn.teams) {
			string ai_filename;
			if (pteam.filename != null) {
				ai_filename = pteam.filename;
			} else {
				assert(ai_index < ais.length);
				ai_filename = ais[ai_index++];
			}

			uint8[] code;
			try {
				FileUtils.get_data(ai_filename, out code);
			} catch (FileError e) {
				warning("Failed to read AI script: %s", e.message);
				return false;
			}

			Team.create(pteam.name, filename, code, pteam.color);
			unowned Team team = Team.lookup(pteam.name);
			assert(team != null);

			foreach (ParsedShip pship in pteam.ships) {
				Ship s = new Ship(pship.class, team, pship.p, vec2(0,0), Game.prng.next_int());

				if (!s.create_ai(null)) {
					warning("Failed to create AI");
					return false;
				}

				Ship.register((owned)s);
			}
		}

		return true;
	}

	ParsedScenario? parse(string filename) {
		if (filename.has_suffix(".json")) {
			return parse_json(filename);
		} else if (filename.has_suffix(".lua")) {
			//return parse_lua(filename);
			return null;
		} else {
			warning("Unexpected scenario filename extension");
			return null;
		}
	}

	ParsedScenario? parse_json(string filename) {
		var scn = new ParsedScenario();
		scn.teams = null;
		scn.num_user_ai = 0;

		string data;
		try {
			FileUtils.get_contents(filename, out data);
		} catch (FileError e) {
			warning("Failed to read scenario: %s", e.message);
			return null;
		}

		var root = cJSON.parse(data);
		if (root == null) {
			warning("Failed to parse scenario");
			return null;
		} else if (root.type != cJSON.Type.Object) {
			warning("root is not an object");
			return null;
		}

		unowned cJSON name = root.objectItem("name");
		if (name == null || name.type != cJSON.Type.String) {
			warning("name field is not a string");
			return null;
		}
		scn.name = name.string;

		unowned cJSON description = root.objectItem("description");
		if (description == null || description.type != cJSON.Type.String) {
			warning("description field is not a string");
			return null;
		}
		scn.description = description.string;

		unowned cJSON teams = root.objectItem("teams");
		if (teams == null || teams.type != cJSON.Type.Object) {
			warning("teams field is not an object");
			return null;
		}

		unowned cJSON team_obj = teams.child;
		while (team_obj != null) {
			if (team_obj.type != cJSON.Type.Object) {
				warning("team definition teams.%s must be an object", team_obj.name);
				return null;
			}

			unowned cJSON color_obj = team_obj.objectItem("color");
			if (color_obj == null || color_obj.type != cJSON.Type.Object) {
				warning("teams.%s.color field is not an object", team_obj.name);
				return null;
			}

			unowned cJSON color_red = color_obj.objectItem("r");
			if (color_red == null || color_red.type != cJSON.Type.Number) {
				warning("teams.%s.color.r field is not a number", team_obj.name);
				return null;
			}
			if (color_red.int < 0 || color_red.int > 255) {
				warning("teams.%s.color.r must be in the range [0,255]", team_obj.name);
				return null;
			}

			unowned cJSON color_green = color_obj.objectItem("g");
			if (color_green == null || color_green.type != cJSON.Type.Number) {
				warning("teams.%s.color.g field is not a number", team_obj.name);
				return null;
			}
			if (color_green.int < 0 || color_green.int > 255) {
				warning("teams.%s.color.g must be in the range [0,255]", team_obj.name);
				return null;
			}

			unowned cJSON color_blue = color_obj.objectItem("b");
			if (color_blue == null || color_blue.type != cJSON.Type.Number) {
				warning("teams.%s.color.b is not a number", team_obj.name);
				return null;
			}
			if (color_blue.int < 0 || color_blue.int > 255) {
				warning("teams.%s.color.b must be in the range [0,255]", team_obj.name);
				return null;
			}

			uint32 color = color_red.int << 24 | color_green.int << 16 | color_blue.int << 8;

			unowned cJSON filename_obj = team_obj.objectItem("filename");
			if (filename_obj != null && filename_obj.type != cJSON.Type.String) {
				warning("teams.%s.color.filename must be a string", filename_obj.name);
				return null;
			}

			var pteam = new ParsedTeam();
			pteam.name = team_obj.name;
			pteam.color = color;
			pteam.filename = (filename_obj != null) ? data_path(filename_obj.string) : null;
			pteam.ships = null;

			unowned cJSON ships = team_obj.objectItem("ships");
			if (ships == null || ships.type != cJSON.Type.Array) {
				warning("teams.%s.ships field is not an array", team_obj.name);
				return null;
			}

			unowned cJSON ship_obj = ships.child;
			int j = 0;
			while (ship_obj != null) {
				if (ship_obj.type != cJSON.Type.Object) {
					warning("ship definition teams.%s.ships[%d] must be an object", team_obj.name, j);
					return null;
				}

				unowned cJSON klass_name = ship_obj.objectItem("class");
				if (klass_name.type != cJSON.Type.String) {
					warning("field teams.%s.ships[%d].class must be a string", team_obj.name, j);
					return null;
				}

				unowned ShipClass klass = ShipClass.lookup(klass_name.string);
				if (klass == null) {
					warning("field teams.%s.ships[%d].class must be a valid ship class", team_obj.name, j);
					return null;
				}

				unowned cJSON x_obj = ship_obj.objectItem("x");
				if (x_obj.type != cJSON.Type.Number) {
					warning("field teams.%s.ships[%d].x must be a number", team_obj.name, j);
					return null;
				}

				unowned cJSON y_obj = ship_obj.objectItem("y");
				if (y_obj.type != cJSON.Type.Number) {
					warning("field teams.%s.ships[%d].y must be a number", team_obj.name, j);
					return null;
				}

				var pship = new ParsedShip();
				pship.class = klass;
				pship.p = vec2(x_obj.double, y_obj.double);
				pteam.ships.append((owned)pship);

				j++;
				ship_obj = ship_obj.next;
			}

			if (pteam.filename != null) {
				scn.num_user_ai++;
			}

			scn.teams.append((owned)pteam);
			team_obj = team_obj.next;
		}

		if (scn.teams.length() < 2) {
			warning("must define at least 2 teams");
			return null;
		}

		return scn;
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

