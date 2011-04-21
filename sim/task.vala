public class RISC.TaskPool {
	[CCode (has_target=false)]
	public delegate void TaskFunc(void *arg1, void *arg2);

	private ThreadPool<TaskData> thread_pool;
	private Cond cvar;
	private Mutex mtx;
	public int in_flight; // XXX atomic

	[Compact]
	public class TaskData {
		public TaskPool pool;
		public TaskFunc f;
		public void *arg1;
		public void *arg2;
	}

	public TaskPool(int thread_pool_size) throws ThreadError {
		if (thread_pool_size > 0) {
			thread_pool = new ThreadPool<TaskData>((Func<TaskData>)static_worker, thread_pool_size, true);
			cvar = new Cond();
			mtx = new Mutex();
			in_flight = 0;
		}
	}

	[CCode (cname = "leak")]
	extern static unowned TaskData leak(owned TaskData t);

	public void run(TaskFunc f, void *arg1, void *arg2) {
		if (thread_pool != null) {
			var t = new TaskData() { pool=this, f=f, arg1=arg1, arg2=arg2 };
			mtx.lock();
			in_flight++;
			mtx.unlock();
			try {
				thread_pool.push(leak((owned)t)); // XXX
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

	~Task() {
		assert(in_flight == 0);
	}

	private static void static_worker(owned TaskData t) {
		var pool = t.pool;
		pool.worker((owned)t);
	}

	private void worker(owned TaskData t) {
		t.f(t.arg1, t.arg2);
		mtx.lock();
		if (--in_flight == 0) {
			cvar.signal();
		}
		mtx.unlock();
	}
}
