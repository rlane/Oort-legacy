using GL;

abstract class Oort.RenderBatch : GLib.Object {
	public Game game;
	public Renderer renderer;
	public RenderPerf perf;

	public RenderBatch() {
		perf = new RenderPerf();
	}

	public void do_render() {
		var start_time = TimeVal();
		render();
		glFinish();
		perf.update_from_time(start_time);
	}

	public abstract void init() throws Error;
	public abstract void render();
}
