using RISC;
using Vector;

public GLib.List<BulletHit> bullet_hits;

[Compact]
public class RISC.BulletHit {
	public unowned Ship s;
	public unowned Bullet b;
	public Vector.Vec2 cp;
	public double e;
}

namespace RISC.Game {
	public int ticks;
	public Rand prng;
	public uint8[] runtime_code;
	public uint8[] ships_code;
	public uint8[] lib_code;

	public List<BulletHit> bullet_hits;

	public const double TICK_LENGTH = 1.0/32;

	static uint8[] load_resource(string name) throws FileError {
		uint8[] data;
		FileUtils.get_data(data_path(name), out data);
		return (owned)data;
	}

	public int init(uint32 seed, ParsedScenario scn, string[] ais) throws FileError {
		prng = new Rand.with_seed(seed);
		runtime_code = load_resource("runtime.lua");
		ships_code = load_resource("ships.lua");
		lib_code = load_resource("lib.lua");
		ticks = 0;

		Task.init(Util.envtol("RISC_NUM_THREADS", 8));
		Bullet.init();
		Ship.init();

		if (!ShipClass.load(data_path("ships.lua"))) {
			return 1;
		}

		if (!Scenario.load(scn, ais)) {
			return 1;
		}

		return 0;
	}

	public void purge() {
		bullet_hits = null;
		Bullet.purge();
		Ship.purge();
	}

	public void tick() {
		check_bullet_hits();
		tick_physics();
		Ship.tick();
		Bullet.tick();
		ticks += 1;
	}

	public void shutdown() {
		Task.shutdown();
		Bullet.shutdown();
		Ship.shutdown();
		Team.shutdown();
		prng = null;
		runtime_code = null;
		ships_code = null;
		lib_code = null;
	}

	public unowned Team? check_victory() {
		unowned Team winner = null;

		foreach (unowned Ship s in Ship.all_ships) {
			if (!s.class.count_for_victory) continue;
			if (winner != null && s.team != winner) {
				return null;
			}
			winner = s.team;
		}

		return winner;
	}

	public void check_bullet_hits() {
		foreach (unowned Ship s in Ship.all_ships) {
			foreach (unowned Bullet b in Bullet.all_bullets) {
				Vec2 cp;
				if (Physics.check_collision(s.physics, b.physics, TICK_LENGTH, out cp)) {
					handle_bullet_hit(s, b, cp);
				}
			}
		}
	}

	public void handle_bullet_hit(Ship s, Bullet b, Vec2 cp) {
		b.dead = true;
		if (b.team != s.team) {
			var dv = s.physics.v.sub(b.physics.v);
			var hit_energy = 0.5 * b.physics.m * dv.abs(); // XXX squared
			s.hull -= hit_energy;
			if (s.hull <= 0) {
				s.dead = true;
			}

			BulletHit hit = new BulletHit();
			hit.s = s;
			hit.b = b;
			hit.cp = cp;
			hit.e = hit_energy;
			bullet_hits.prepend((owned) hit);
		}
	}

	public void tick_physics() {
		foreach (unowned Ship s in Ship.all_ships) {
			s.physics.tick_one();
		}

		foreach (unowned Bullet b in Bullet.all_bullets) {
			b.physics.tick_one();
		}
	}
}
