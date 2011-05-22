using Oort;
using Vector;

public enum Oort.BulletType {
	SLUG = 1,
	PLASMA = 2,
	EXPLOSION = 3,
	REFUEL = 4,
}

[Compact]
public class Oort.Bullet {
	public uint32 shooter_id;
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
