public class Oort.RenderPerf {
	const int BUCKETS = 20;
	const int BUCKET_SIZE = 5; // ms

	public double frame_time_avg = 0.0;
	double frame_time_min = 100000;
	double frame_time_max = 0;
	int num_frames = 0;
	int[] histogram;

	public RenderPerf() {
		histogram = new int[BUCKETS];
	}

	public void update(double frame_time) {
		if (num_frames == 0) {
			frame_time_avg = frame_time;
		} else {
			frame_time_avg = (frame_time_avg*31.0 + frame_time)/32.0;
		}
		num_frames++;
		frame_time_min = double.min(frame_time, frame_time_min);
		frame_time_max = double.max(frame_time, frame_time_max);
		int bucket = (int) (frame_time/BUCKET_SIZE);
		if (bucket >= BUCKETS) {
			bucket = BUCKETS - 1;
		}
		histogram[bucket]++;
	}

	public void update_from_time(TimeVal start_time) {
		const int million = 1000*1000;
		TimeVal end_time = TimeVal();
		long usecs = (end_time.tv_sec-start_time.tv_sec)*million + (end_time.tv_usec - start_time.tv_usec);
		update(usecs/1000.0);
	}

	public void dump() {
		print("num frames: %d\n", num_frames);
		print("avg time: %f ms\n", frame_time_avg);
		print("min time: %f ms\n", frame_time_min);
		print("max time: %f ms\n", frame_time_max);
		print("time histogram:\n");
		for (int i = 0; i < BUCKETS; i++) {
			if (histogram[i] > 0) {
				print("%d-%d ms: %d\n", i*BUCKET_SIZE, (i+1)*BUCKET_SIZE, histogram[i]);
			}
		}
	}

	public string summary() {
		return "avg=%.1fms min=%.1fms max=%.1fms".printf(frame_time_avg, frame_time_min, frame_time_max);
	}
}
