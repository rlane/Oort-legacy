// Copyright 2011 Rich Lane

#ifndef OORT_SIM_MATH_UTIL_H_
#define OORT_SIM_MATH_UTIL_H_

#include <Box2D/Box2D.h>

float normalize_angle(float a) {
	while (a < -M_PI) a += 2*M_PI;
	while (a > M_PI) a -= 2*M_PI;
	return a;
}

float angle_diff(float a, float b) {
	float c = normalize_angle(b - a);
	if (c > M_PI) {
		c -= 2*M_PI;
	}
	return c;
}

float angle_between(b2Vec2 a, b2Vec2 b) {
	auto c = b - a;
	return atan2(c.y, c.x);
}

#endif
