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
	[CCode (cname = "ticks")]
	public int ticks;
	[CCode (cname = "prng")]
	public Rand prng;

	public List<BulletHit> bullet_hits;

	public int init(int seed, string scenario, string[] ais) {
		prng = new Rand.with_seed(seed);
		ticks = 0;

		Task.init(C.envtol("RISC_NUM_THREADS", 8));
		Bullet.init();
		Physics.init();

		if (!ShipClass.load(data_path("ships.lua"))) {
			return 1;
		}

		if (!Scenario.load(scenario, ais)) {
			return 1;
		}

		return 0;
	}

	public void purge() {
		bullet_hits = null;
		Bullet.purge();
		Ship.purge();
	}

	public void tick(double tick_length) {
		check_bullet_hits(tick_length);
		Physics.tick(tick_length);
		Ship.tick(tick_length);
		Bullet.tick(tick_length);
		ticks += 1;
	}

	public void shutdown() {
		Bullet.shutdown();
		Ship.shutdown();
		Team.shutdown();
		Task.shutdown();
	}

	public unowned Team? check_victory() {
		unowned Team winner = null;

		foreach (unowned Ship s in RISC.all_ships) {
			if (!s.class.count_for_victory) continue;
			if (winner != null && s.team != winner) {
				return null;
			}
			winner = s.team;
		}

		return winner;
	}

	public void check_bullet_hits(double tick_length) {
		foreach (unowned Ship s in all_ships) {
			foreach (unowned Bullet b in Bullet.all_bullets) {
				Vec2 cp;
				if (Physics.check_collision(s.physics, b.physics, tick_length, out cp)) {
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
}
