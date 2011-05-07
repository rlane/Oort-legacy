using Oort;
using Vector;

public enum Oort.BeamGraphics {
	ION = 1,
	LASER = 2,
}

[Compact]
public class Oort.Beam {
	public unowned Team team;
	public Vec2 p;
	public double a;
	public double length;
	public double width;
	public double damage;
	public BeamGraphics graphics;
}
