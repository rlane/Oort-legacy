using RISC;

namespace RISC.Game {
	public int init(int seed, string scenario, string[] ais) {
		return CGame.init(seed, scenario, ais);
	}

	public void purge() {
		CGame.purge();
	}

	public void tick(double tick_length) {
		CGame.tick(tick_length);
	}

	public void shutdown() {
		CGame.shutdown();
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
