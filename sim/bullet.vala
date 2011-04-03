using RISC;
using Vector;

public enum RISC.BulletType {
	SLUG = 1,
	PLASMA = 2,
	EXPLOSION = 3,
	ION_BEAM = 4,
}

[Compact]
public class RISC.Bullet {
	public Physics physics;
	public unowned Team team;
	public double ttl;
	public bool dead;
	public BulletType type;

	public void tick() {
		ttl -= Game.TICK_LENGTH;
		if (ttl <= 0) dead = true;
	}
}
