using GL;

abstract class Oort.RenderBatch {
	public Game game;
	public Renderer renderer;
	public abstract void init() throws Error;
	public abstract void render();
}
