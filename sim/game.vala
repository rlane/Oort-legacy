using Oort;
using Vector;

[Compact]
public class Oort.BulletHit {
	public unowned Ship s;
	public unowned Bullet b;
	public Vector.Vec2 cp;
	public double e;
}

[Compact]
public class Oort.BeamHit {
	public unowned Ship s;
	public unowned Beam b;
	public Vector.Vec2 cp;
	public double e;
}

public class Oort.Game {
	public int ticks = 0;
	public Rand prng;
	public uint8[] runtime_code;
	public uint8[] ships_code;
	public uint8[] lib_code;
	public uint8[] strict_code;
	public uint8[] vector_code;
	public ParsedScenario scn;
	public string[] ais;
	public List<BulletHit> bullet_hits = null;
	public List<BeamHit> beam_hits = null;
	public List<Ship> all_ships = null;
	public List<Ship> new_ships = null;
	public Mutex new_ships_lock;
	public Mutex radio_lock;
	public GLib.FileStream trace_file = null;
	public List<Bullet> all_bullets;
	public List<Bullet> new_bullets;
	public Mutex new_bullets_lock;
	public List<Beam> all_beams;
	public List<Beam> new_beams;
	public Mutex new_beams_lock;
	public List<Team> teams;
	public TaskPool tasks;
	public Mutex log_lock;

	public const double TICK_LENGTH = 1.0/32;

	static uint8[] load_resource(string name) throws FileError {
		uint8[] data;
		FileUtils.get_data(data_path(name), out data);
		return (owned)data;
	}

	public Game(uint32 seed, ParsedScenario scn, string[] ais) throws FileError, ThreadError, ScenarioLoadError {
		prng = new Rand.with_seed(seed);
		this.scn = scn;
		this.ais = ais;
		new_ships_lock = new Mutex();
		new_bullets_lock = new Mutex();
		new_beams_lock = new Mutex();
		radio_lock = new Mutex();
		log_lock = new Mutex();
		runtime_code = load_resource("runtime.lua");
		ships_code = load_resource("ships.lua");
		lib_code = load_resource("lib.lua");
		strict_code = load_resource("strict.lua");
		vector_code = load_resource("vector.lua");
		tasks = new TaskPool(Util.envtol("Oort_NUM_THREADS", 8));
		Scenario.load(this, scn, ais);
	}

	public void purge() {
		bullet_hits = null;
		beam_hits = null;
		purge_bullets();
		purge_ships();
	}

	public void tick() {
		check_bullet_hits();
		check_beam_hits();
		tick_physics();
		tick_bullets();
		tick_ships();
		all_bullets.concat((owned) new_bullets);
		new_bullets = null;
		all_beams = (owned) new_beams;
		new_beams = null;
		ticks += 1;
	}

	public unowned Team? check_victory() {
		unowned Team winner = null;

		foreach (unowned Ship s in all_ships) {
			if (!s.class.count_for_victory) continue;
			if (s.physics.p.distance(vec2(0,0)) > scn.radius) continue;
			if (winner != null && s.team != winner) {
				return null;
			}
			winner = s.team;
		}

		return winner;
	}

	public Team? lookup_team(string name) {
		foreach (Team team in teams) {
			if (team.name == name) return team;
		}
		return null;
	}

	public void check_bullet_hits() {
		foreach (unowned Ship s in all_ships) {
			foreach (unowned Bullet b in all_bullets) {
				Vec2 cp;
				if (Physics.check_collision(s.physics, b.physics, TICK_LENGTH, out cp)) {
					handle_bullet_hit(s, b, cp);
				}
			}
		}
	}

	public void handle_bullet_hit(Ship s, Bullet b, Vec2 cp) {
		if (b.shooter_id == s.api_id) {
			return;
		}

		b.dead = true;

		if (b.type != BulletType.REFUEL && b.team != s.team) {
			var dv = s.physics.v.sub(b.physics.v);
			var hit_energy = 0.5 * b.physics.m * dv.abs() * dv.abs();
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
		} else if (b.type == BulletType.REFUEL) {
			s.refuel_hit(b.physics.m);
		}
	}

	public void check_beam_hits() {
		foreach (unowned Ship s in all_ships) {
			foreach (unowned Beam b in all_beams) {
				Vec2 cp;
				if (Physics.check_beam_collision(s.physics, b, TICK_LENGTH, out cp)) {
					handle_beam_hit(s, b, cp);
				}
			}
		}
	}

	public void handle_beam_hit(Ship s, Beam b, Vec2 cp) {
		if (b.team != s.team) {
			s.hull -= b.damage * TICK_LENGTH;
			if (s.hull <= 0) {
				s.dead = true;
			}

			BeamHit hit = new BeamHit();
			hit.s = s;
			hit.b = b;
			hit.cp = cp;
			hit.e = b.damage*TICK_LENGTH;
			beam_hits.prepend((owned) hit);
		}
	}

	void tick_physics() {
		foreach (unowned Ship s in all_ships) {
			s.physics.tick_one();
		}

		foreach (unowned Bullet b in all_bullets) {
			b.physics.tick_one();
		}
	}

	void tick_ships() {
		new_ships.sort((CompareFunc)Ship.compare);
		all_ships.concat((owned) new_ships);
		new_ships = null;

		foreach (unowned Ship s in all_ships) {
			tasks.run((TaskPool.TaskFunc)s.tick, s, null);
		}
		tasks.wait();
	}

	void tick_bullets() {
		foreach (unowned Bullet b in all_bullets) {
			b.tick();
		}
	}

	[CCode (cname = "leak")]
	static extern Ship unleak_ship(Ship s);

	void purge_ships() {
		unowned List<Ship> cur = all_ships;
		while (cur != null) {
			unowned List<Ship> next = cur.next;
			if (cur.data.dead) {
				unleak_ship(cur.data);
				all_ships.delete_link(cur);
			}
			cur = next;
		}
	}

	[CCode (cname = "leak")]
	static extern Bullet unleak_bullet(Bullet b);

	void purge_bullets() {
		unowned List<Bullet> cur = all_bullets;
		while (cur != null) {
			unowned List<Bullet> next = cur.next;
			if (cur.data.dead) {
				unleak_bullet(cur.data);
				all_bullets.delete_link(cur);
			}
			cur = next;
		}
	}
}
