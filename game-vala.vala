using RISC;

namespace RISC.Game {
	[CCode (cname = "ticks")]
	public int ticks;

	public int init(int seed, string scenario, string[] ais) {
		return CGame.init(seed, scenario, ais);
	}

	public void purge() {
		CGame.purge();
		Bullet.purge();
		Ship.purge();
	}

	public void tick(double tick_length) {
		CGame.tick(tick_length);
		Physics.tick(tick_length);
		Bullet.tick(tick_length);
		Ship.tick(tick_length);
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
}
