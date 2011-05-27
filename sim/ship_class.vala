using Lua;

[Compact]
public class Oort.ShipClass {
	public string name;
	public double radius;
	public double hull;
	public bool count_for_victory;
	public double mass;
	public double reaction_mass;
	public int cpu;

	static HashTable<string,ShipClass> ship_classes;

	public static bool load(string filename) {
		var L = new LuaVM();

		if (L.do_file(filename)) {
			warning("Failed to load ships from %s: %s\n", filename, L.to_string(-1));
			return false;
		}

		L.get_global("ships");

		if (L.is_nil(1)) {
			warning("Failed to load ships from %s: 'ships' table not defined\n", filename);
			return false;
		}

		ship_classes = new HashTable<string,ShipClass>(str_hash, str_equal);

		L.push_nil();
		while (L.next(1) != 0) {
			var name = L.to_string(-2);
			var c = new ShipClass();
			c.name = name;
			c.radius = L.get_field_number(-1, "radius");
			c.hull = L.get_field_number(-1, "hull");
			L.get_field(-1, "count_for_victory");
			c.count_for_victory = L.to_boolean(-1);
			L.pop(1);
			c.mass = L.get_field_number(-1, "mass");
			c.reaction_mass = L.get_field_number(-1, "reaction_mass");
			c.cpu = (int)L.get_field_number(-1, "cpu");
			ship_classes.insert(name, (owned)c);
			L.pop(1);
		}

		return true;
	}

	public static unowned ShipClass lookup(string name) {
		return ship_classes.lookup(name);
	}
}
