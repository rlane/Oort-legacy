[Compact]
public class RISC.Team {
	public uint32 color;
	public string name;
	public string filename;
	public int ships;

	public static List<Team> all_teams;

	public static void create(string name, string filename, uint32 color) {
		var team = new Team() { name=name, filename=filename, color=color, ships=0 };
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
