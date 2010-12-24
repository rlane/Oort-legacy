namespace RISC.Task {
	[CCode (has_target=false)]
	public delegate void TaskFunc(void *arg1, void *arg2);

	private ThreadPool thread_pool;
	private Cond cvar;
	private Mutex mtx;
	private int in_flight;

	[Compact]
	public class TaskData {
		public TaskFunc f;
		public void *arg1;
		public void *arg2;
	}

	public void init(int thread_pool_size) {
		if (thread_pool_size > 0) {
			try {
				thread_pool = new ThreadPool((Func)worker, thread_pool_size, true);
			} catch (ThreadError e) {
				error(e.message);
			}
			cvar = new Cond();
			mtx = new Mutex();
			in_flight = 0;
		}
	}

	public void task(TaskFunc f, void *arg1, void *arg2) {
		if (thread_pool != null) {
			var t = new TaskData() { f=f, arg1=arg1, arg2=arg2 };
			mtx.lock();
			in_flight++;
			mtx.unlock();
			void *p = leak((owned)t); // HACK
			try {
				thread_pool.push(p);
			} catch (ThreadError e) {
				error(e.message);
			}
		} else {
			f(arg1, arg2);
		}
	}

	public void wait() {
		if (thread_pool != null) {
			mtx.lock();
			while (in_flight > 0)
				cvar.wait(mtx);
			mtx.unlock();
		}
	}

	public void shutdown()
	{
		assert(in_flight == 0);
		thread_pool = null;
		cvar = null;
		mtx = null;
	}

	private void worker(owned TaskData t) {
		t.f(t.arg1, t.arg2);
		mtx.lock();
		in_flight--;
		cvar.signal(); // XXX signal when 0
		mtx.unlock();
	}

	[CCode (cname = "leak")]
	extern void *leak(owned TaskData t);
}
