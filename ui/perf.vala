class Oort.RenderPerf {
	const int BUCKETS = 20;
	const int BUCKET_SIZE = 5; // ms

	double frame_time_min = 100000;
	double frame_time_max = 0;
	int num_frames = 0;
	double frame_time_total = 0.0;
	int[] histogram;

	public RenderPerf() {
		histogram = new int[BUCKETS];
	}

	public void update(double frame_time) {
		num_frames++;
		frame_time_total += frame_time;
		frame_time_min = double.min(frame_time, frame_time_min);
		frame_time_max = double.max(frame_time, frame_time_max);
		int bucket = (int) (frame_time/BUCKET_SIZE);
		if (bucket >= BUCKETS) {
			bucket = BUCKETS - 1;
		}
		histogram[bucket]++;
	}

	public void dump() {
		print("num frames: %d\n", num_frames);
		print("avg time: %f ms\n", frame_time_total/num_frames);
		print("min time: %f ms\n", frame_time_min);
		print("max time: %f ms\n", frame_time_max);
		print("time histogram:\n");
		for (int i = 0; i < BUCKETS; i++) {
			print("%d-%d ms: %d\n", i*BUCKET_SIZE, (i+1)*BUCKET_SIZE, histogram[i]);
		}
	}
}
