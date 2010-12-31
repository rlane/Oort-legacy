[Compact]
public class RISC.Team {
	public uint32 color;
	public string name;
	public string filename;
	public uint8[] code;
	public int ships;

	public static List<Team> all_teams;

	public static void create(string name, string? filename, uint8[] code, uint32 color) {
		var team = new Team() { name=name, color=color, ships=0, filename=filename, code=code };
		all_teams.append((owned) team);
	}

	public static void shutdown() {
		all_teams = null;
	}

	public static unowned Team? lookup(string name) {
		foreach (unowned Team team in all_teams) {
			if (team.name == name) return team;
		}
		return null;
	}
}
