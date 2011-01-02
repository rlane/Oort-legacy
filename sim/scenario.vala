using RISC;
using Lua;
using Vector;

/*
public errordomain RISC.ScenarioParseError {
}
*/

public errordomain RISC.ScenarioLoadError {
	NUM_USER_AI,
	INVALID_SHIP_CLASS,
	FAILED_AI_CREATION,
}

public class RISC.ParsedScenario {
	public string name;
	public string description;
	public List<ParsedTeam> teams;
	public List<ParsedTeam> user_teams;
}

public class RISC.ParsedTeam {
	public string name;
	public string filename;
	public uint8 color_red;
	public uint8 color_green;
	public uint8 color_blue;
	public List<ParsedShip> ships;
}

public class RISC.ParsedShip {
	public string class_name;
	public Vec2 p;
}

namespace RISC.Scenario {
	public void load(Game game, ParsedScenario scn, string[] ais) throws ScenarioLoadError, FileError {
		if (scn.user_teams.length() != ais.length) {
			throw new ScenarioLoadError.NUM_USER_AI("Expected %u user AIs, got %d", scn.user_teams.length(), ais.length);
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
			FileUtils.get_data(ai_filename, out code);
			uint32 color = pteam.color_red << 24 | pteam.color_green << 16 | pteam.color_blue << 8;
			var team = new Team() { name=pteam.name, color=color, filename=ai_filename, code=(owned)code };

			foreach (ParsedShip pship in pteam.ships) {
				unowned ShipClass klass = ShipClass.lookup(pship.class_name);
				if (klass == null) {
					throw new ScenarioLoadError.INVALID_SHIP_CLASS("Invalid ship class '%s'", pship.class_name);
				}

				Ship s = new Ship(game, klass, team, pship.p, vec2(0,0), game.prng.next_int());

				if (!s.create_ai(null)) {
					throw new ScenarioLoadError.FAILED_AI_CREATION("Failed to create AI");
				}

				game.new_ships.append((owned)s);
			}

			game.teams.append((owned)team);
		}
	}

	public ParsedScenario? parse(string filename) {
		var scn = new ParsedScenario();
		scn.teams = null;
		scn.user_teams = null;

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
		if (teams == null || teams.type != cJSON.Type.Array) {
			warning("teams field is not an array");
			return null;
		}

		int i = 0;
		unowned cJSON team_obj = teams.child;
		while (team_obj != null) {
			if (team_obj.type != cJSON.Type.Object) {
				warning("team definition teams[%d] must be an object", i);
				return null;
			}

			unowned cJSON team_name = team_obj.objectItem("name");
			if (team_name == null || team_name.type != cJSON.Type.String) {
				warning("teams[%d].name field is not a string", i);
				return null;
			}

			unowned cJSON color_obj = team_obj.objectItem("color");
			if (color_obj == null || color_obj.type != cJSON.Type.Object) {
				warning("teams.%s.color field is not an object", team_name.string);
				return null;
			}

			unowned cJSON color_red = color_obj.objectItem("r");
			if (color_red == null || color_red.type != cJSON.Type.Number) {
				warning("teams.%s.color.r field is not a number", team_name.string);
				return null;
			}
			if (color_red.int < 0 || color_red.int > 255) {
				warning("teams.%s.color.r must be in the range [0,255]", team_name.string);
				return null;
			}

			unowned cJSON color_green = color_obj.objectItem("g");
			if (color_green == null || color_green.type != cJSON.Type.Number) {
				warning("teams.%s.color.g field is not a number", team_name.string);
				return null;
			}
			if (color_green.int < 0 || color_green.int > 255) {
				warning("teams.%s.color.g must be in the range [0,255]", team_name.string);
				return null;
			}

			unowned cJSON color_blue = color_obj.objectItem("b");
			if (color_blue == null || color_blue.type != cJSON.Type.Number) {
				warning("teams.%s.color.b is not a number", team_name.string);
				return null;
			}
			if (color_blue.int < 0 || color_blue.int > 255) {
				warning("teams.%s.color.b must be in the range [0,255]", team_name.string);
				return null;
			}

			unowned cJSON filename_obj = team_obj.objectItem("filename");
			if (filename_obj != null && filename_obj.type != cJSON.Type.String) {
				warning("teams.%s.color.filename must be a string", filename_obj.name);
				return null;
			}

			var pteam = new ParsedTeam();
			pteam.name = team_name.string;
			pteam.color_red = (uint8)color_red.int;
			pteam.color_green = (uint8)color_green.int;
			pteam.color_blue = (uint8)color_blue.int;
			pteam.filename = (filename_obj != null) ? data_path(filename_obj.string) : null;
			pteam.ships = null;

			unowned cJSON ships = team_obj.objectItem("ships");
			if (ships == null || ships.type != cJSON.Type.Array) {
				warning("teams.%s.ships field is not an array", team_name.string);
				return null;
			}

			unowned cJSON ship_obj = ships.child;
			int j = 0;
			while (ship_obj != null) {
				if (ship_obj.type != cJSON.Type.Object) {
					warning("ship definition teams.%s.ships[%d] must be an object", team_name.string, j);
					return null;
				}

				unowned cJSON class_name = ship_obj.objectItem("class");
				if (class_name.type != cJSON.Type.String) {
					warning("field teams.%s.ships[%d].class must be a string", team_name.string, j);
					return null;
				}

				unowned cJSON x_obj = ship_obj.objectItem("x");
				if (x_obj.type != cJSON.Type.Number) {
					warning("field teams.%s.ships[%d].x must be a number", team_name.string, j);
					return null;
				}

				unowned cJSON y_obj = ship_obj.objectItem("y");
				if (y_obj.type != cJSON.Type.Number) {
					warning("field teams.%s.ships[%d].y must be a number", team_name.string, j);
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
				scn.user_teams.append(pteam);
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
