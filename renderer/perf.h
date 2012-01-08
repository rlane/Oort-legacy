// Copyright 2011 Rich Lane
#ifndef OORT_RENDERER_PERF_H_
#define OORT_RENDERER_PERF_H_

#include <boost/format.hpp>
#include "common/log.h"
#include <sys/time.h>

namespace Oort {

inline uint64_t microseconds()
{
#ifdef __native_client__
	struct timeval tv;
	if (gettimeofday(&tv, NULL)) {
		perror("gettimeofday");
		abort();
	}
	return (uint64_t)tv.tv_sec * 1000000LL + (uint64_t)tv.tv_usec;
#else
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (uint64_t)ts.tv_sec * 1000000LL + (uint64_t)ts.tv_nsec / 1000LL;
#endif
}

// Microsecond timer
class Timer {
public:
	uint64_t start;

	Timer()
		: start(microseconds()) {}

	uint64_t elapsed() const {
		return microseconds() - start;
	}
};

struct PerfHistogram {
	enum {
		NUM_BUCKETS = 20,
		BUCKET_SIZE = 5
	};

	int num_frames;
	int buckets[NUM_BUCKETS];
	float min, max, sum, mavg;

	PerfHistogram() {
		num_frames = 0;
		min = 1000;
		max = 0;
		sum = 0;
		mavg = 0;

		for (int i = 0; i < NUM_BUCKETS; i++) {
			buckets[i] = 0.0f;
		}
	}

	void update(float ms) {
		if (num_frames == 0) {
			mavg = ms;
		} else {
			mavg = (63.0f*mavg + ms)/64.0f;
		}

		num_frames++;
		sum += ms;
		if (ms < min) min = ms;
		if (ms > max) max = ms;
		int idx = int(ms/BUCKET_SIZE);
		if (idx >= NUM_BUCKETS) idx = NUM_BUCKETS - 1;
		buckets[idx]++;
	}

	void update(const Timer &t) {
		float ms = float(t.elapsed()) / 1000.0f;
		update(ms);
	}

	void dump() {
		log("num frames: %d", num_frames);
		log("avg time: %f ms", sum/num_frames);
		log("mavg time: %f ms", mavg);
		log("min time: %f ms", min);
		log("max time: %f ms", max);
		log("time histogram:");
		for (int i = 0; i < NUM_BUCKETS; i++) {
			if (buckets[i] > 0) {
				log("%d-%d ms: %d", i*BUCKET_SIZE, (i+1)*BUCKET_SIZE, buckets[i]);
			}
		}
	}

	std::string summary() {
		boost::format fmt("mavg=%.2fms avg=%.2fms min=%.2fms max=%.2fms");
		fmt % mavg % (sum/num_frames) % min % max;
		return fmt.str();
	}
};

}

#endif
