using Oort;
using Lua;
using Vector;

public errordomain Oort.ScenarioParseError {
	JSON_SYNTAX,
	WRONG_TYPE,
	BAD_VALUE,
	NUM_TEAMS,
}

public errordomain Oort.ScenarioLoadError {
	NUM_USER_AI,
	INVALID_SHIP_CLASS,
	FAILED_AI_CREATION,
}

public class Oort.ParsedScenario {
	public string name;
	public string description;
	public double initial_view_scale;
	public double radius;
	public List<ParsedTeam> teams;
	public List<ParsedTeam> user_teams;
}

public class Oort.ParsedTeam {
	public int id;
	public string name;
	public uint8[]? code;
	public uint8 color_red;
	public uint8 color_green;
	public uint8 color_blue;
	public List<ParsedShip> ships;
}

public class Oort.ParsedShip {
	public string class_name;
	public Vec2 p;
	public double h;
}

namespace Oort.Scenario {
	public void load(Game game, ParsedScenario scn, string[] ais) throws ScenarioLoadError, FileError {
		if (scn.user_teams.length() != ais.length) {
			throw new ScenarioLoadError.NUM_USER_AI("Expected %u user AIs, got %d", scn.user_teams.length(), ais.length);
		}

		int ai_index = 0;
		foreach (ParsedTeam pteam in scn.teams) {
			uint8[] code;
			string ai_filename;
			if (pteam.code != null) {
				code = pteam.code;
				ai_filename = "inline";
			} else {
				assert(ai_index < ais.length);
				ai_filename = ais[ai_index++];
				code = Resources.load(ai_filename); // XXX
			}

			uint32 color = pteam.color_red << 24 | pteam.color_green << 16 | pteam.color_blue << 8;
			var team = new Team() { id=pteam.id, name=pteam.name, color=color, code=(owned)code, filename=ai_filename };

			foreach (ParsedShip pship in pteam.ships) {
				unowned ShipClass klass = ShipClass.lookup(pship.class_name);
				if (klass == null) {
					throw new ScenarioLoadError.INVALID_SHIP_CLASS("Invalid ship class '%s'", pship.class_name);
				}

				Ship s = new Ship(game, klass, team, pship.p, vec2(0,0), pship.h, game.prng.next_int());

				if (!s.create_ai(null)) {
					throw new ScenarioLoadError.FAILED_AI_CREATION("Failed to create AI");
				}

				game.new_ships.append((owned)s);
			}

			game.teams.append((owned)team);
		}
	}

	public ParsedScenario? parse(string filename) throws FileError, ScenarioParseError {
		var scn = new ParsedScenario();
		scn.teams = null;
		scn.user_teams = null;

		uint8[] data = Resources.load(filename); // XXX

		var root = cJSON.parse((string)data);
		if (root == null) {
			throw new ScenarioParseError.JSON_SYNTAX("JSON syntax incorrect");
		} else if (root.type != cJSON.Type.Object) {
			throw new ScenarioParseError.WRONG_TYPE("root must be an object");
		}

		unowned cJSON name = root.objectItem("name");
		if (name == null || name.type != cJSON.Type.String) {
			throw new ScenarioParseError.WRONG_TYPE("name field must be a string");
		}
		scn.name = name.string;

		unowned cJSON description = root.objectItem("description");
		if (description == null || description.type != cJSON.Type.String) {
			throw new ScenarioParseError.WRONG_TYPE("description field must be a string");
		}
		scn.description = description.string;

		unowned cJSON initial_view_scale = root.objectItem("initial_view_scale");
		if (initial_view_scale != null) {
			if (initial_view_scale.type != cJSON.Type.Number) {
				throw new ScenarioParseError.WRONG_TYPE("initial_view_scale field must be a number");
			} else {
				scn.initial_view_scale = initial_view_scale.double;
			}
		} else {
			scn.initial_view_scale = 0.2;
		}

		unowned cJSON radius = root.objectItem("radius");
		if (radius == null || radius.type != cJSON.Type.Number) {
			throw new ScenarioParseError.WRONG_TYPE("radius field must be a number");
		} else {
			scn.radius = radius.double;
		}

		unowned cJSON teams = root.objectItem("teams");
		if (teams == null || teams.type != cJSON.Type.Array) {
			throw new ScenarioParseError.WRONG_TYPE("teams field must be an array");
		}

		int i = 0;
		unowned cJSON team_obj = teams.child;
		while (team_obj != null) {
			if (team_obj.type != cJSON.Type.Object) {
				throw new ScenarioParseError.WRONG_TYPE("team definition teams[%d] must be an object", i);
			}

			unowned cJSON team_name = team_obj.objectItem("name");
			if (team_name == null || team_name.type != cJSON.Type.String) {
				throw new ScenarioParseError.WRONG_TYPE("teams[%d].name field is not a string", i);
			}

			unowned cJSON color_obj = team_obj.objectItem("color");
			if (color_obj == null || color_obj.type != cJSON.Type.Object) {
				throw new ScenarioParseError.WRONG_TYPE("teams.%s.color field is not an object", team_name.string);
			}

			unowned cJSON color_red = color_obj.objectItem("r");
			if (color_red == null || color_red.type != cJSON.Type.Number) {
				throw new ScenarioParseError.WRONG_TYPE("teams.%s.color.r field is not a number", team_name.string);
			}
			if (color_red.int < 0 || color_red.int > 255) {
				throw new ScenarioParseError.BAD_VALUE("teams.%s.color.r must be in the range [0,255]", team_name.string);
			}

			unowned cJSON color_green = color_obj.objectItem("g");
			if (color_green == null || color_green.type != cJSON.Type.Number) {
				throw new ScenarioParseError.WRONG_TYPE("teams.%s.color.g field is not a number", team_name.string);
			}
			if (color_green.int < 0 || color_green.int > 255) {
				throw new ScenarioParseError.BAD_VALUE("teams.%s.color.g must be in the range [0,255]", team_name.string);
			}

			unowned cJSON color_blue = color_obj.objectItem("b");
			if (color_blue == null || color_blue.type != cJSON.Type.Number) {
				throw new ScenarioParseError.WRONG_TYPE("teams.%s.color.b is not a number", team_name.string);
			}
			if (color_blue.int < 0 || color_blue.int > 255) {
				throw new ScenarioParseError.BAD_VALUE("teams.%s.color.b must be in the range [0,255]", team_name.string);
			}

			unowned cJSON code_obj = team_obj.objectItem("code");
			if (code_obj != null && code_obj.type != cJSON.Type.String) {
				throw new ScenarioParseError.WRONG_TYPE("teams.%s.color.code must be a string", code_obj.name);
			}

			var pteam = new ParsedTeam();
			pteam.id = i;
			pteam.name = team_name.string;
			pteam.color_red = (uint8)color_red.int;
			pteam.color_green = (uint8)color_green.int;
			pteam.color_blue = (uint8)color_blue.int;
			pteam.code = (code_obj != null) ? code_obj.string.data : null;
			pteam.ships = null;

			unowned cJSON ships = team_obj.objectItem("ships");
			if (ships == null || ships.type != cJSON.Type.Array) {
				throw new ScenarioParseError.WRONG_TYPE("teams.%s.ships field is not an array", team_name.string);
			}

			unowned cJSON ship_obj = ships.child;
			int j = 0;
			while (ship_obj != null) {
				if (ship_obj.type != cJSON.Type.Object) {
					throw new ScenarioParseError.WRONG_TYPE("ship definition teams.%s.ships[%d] must be an object", team_name.string, j);
				}

				unowned cJSON class_name = ship_obj.objectItem("class");
				if (class_name == null || class_name.type != cJSON.Type.String) {
					throw new ScenarioParseError.WRONG_TYPE("field teams.%s.ships[%d].class must be a string", team_name.string, j);
				}

				unowned cJSON x_obj = ship_obj.objectItem("x");
				if (x_obj == null || x_obj.type != cJSON.Type.Number) {
					throw new ScenarioParseError.WRONG_TYPE("field teams.%s.ships[%d].x must be a number", team_name.string, j);
				}

				unowned cJSON y_obj = ship_obj.objectItem("y");
				if (y_obj == null || y_obj.type != cJSON.Type.Number) {
					throw new ScenarioParseError.WRONG_TYPE("field teams.%s.ships[%d].y must be a number", team_name.string, j);
				}

				unowned cJSON h_obj = ship_obj.objectItem("h");
				if (h_obj == null || h_obj.type != cJSON.Type.Number) {
					throw new ScenarioParseError.WRONG_TYPE("field teams.%s.ships[%d].h must be a number", team_name.string, j);
				}

				var pship = new ParsedShip();
				pship.class_name = class_name.string;
				pship.p = vec2(x_obj.double, y_obj.double);
				pship.h = h_obj.double;
				pteam.ships.append((owned)pship);

				j++;
				ship_obj = ship_obj.next;
			}

			if (pteam.code == null) {
				scn.user_teams.append(pteam);
			}

			scn.teams.append((owned)pteam);
			i++;
			team_obj = team_obj.next;
		}

		if (scn.teams.length() < 2) {
			throw new ScenarioParseError.NUM_TEAMS("must define at least 2 teams");
		}

		return scn;
	}
}
