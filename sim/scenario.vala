using RISC;
using Lua;
using Vector;

namespace RISC {
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
		public string class_name;
		public Vec2 p;
	}
}

namespace RISC.Scenario {
	public bool load(ParsedScenario scn, string[] ais) {
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

			Team.create(pteam.name, ai_filename, code, pteam.color);
			unowned Team team = Team.lookup(pteam.name);
			assert(team != null);

			foreach (ParsedShip pship in pteam.ships) {
				unowned ShipClass klass = ShipClass.lookup(pship.class_name);
				if (klass == null) {
					warning("Invalid ship class '%s'", pship.class_name);
					return false;
				}

				Ship s = new Ship(klass, team, pship.p, vec2(0,0), Game.prng.next_int());

				if (!s.create_ai(null)) {
					warning("Failed to create AI");
					return false;
				}

				Ship.register((owned)s);
			}
		}

		return true;
	}

	public ParsedScenario? parse(string filename) {
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

				unowned cJSON class_name = ship_obj.objectItem("class");
				if (class_name.type != cJSON.Type.String) {
					warning("field teams.%s.ships[%d].class must be a string", team_obj.name, j);
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
				pship.class_name = class_name.string;
				pship.p = vec2(x_obj.double, y_obj.double);
				pteam.ships.append((owned)pship);

				j++;
				ship_obj = ship_obj.next;
			}

			if (pteam.filename == null) {
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
}
