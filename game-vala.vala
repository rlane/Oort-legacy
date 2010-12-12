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

	public unowned Team check_victory() {
		return CGame.check_victory();
	}
}
