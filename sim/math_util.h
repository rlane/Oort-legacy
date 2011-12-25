// Copyright 2011 Rich Lane

#ifndef OORT_SIM_MATH_UTIL_H_
#define OORT_SIM_MATH_UTIL_H_

#include <Box2D/Box2D.h>
#include "glm/glm.hpp"

const float pi = 3.141592653589793238f;

namespace Oort {
	enum {
		SCALE = 100
	};
}

static inline float normalize_angle(float a) {
	while (a < -pi) a += 2*pi;
	while (a > pi) a -= 2*pi;
	return a;
}

static inline float angle_diff(float a, float b) {
	float c = normalize_angle(b - a);
	if (c > pi) {
		c -= 2*pi;
	}
	return c;
}

static inline float angle_between(glm::vec2 a, glm::vec2 b) {
	auto c = b - a;
	return atan2(c.y, c.x);
}

static inline b2Vec2 n2b(glm::vec2 vec) {
	return (1.0/Oort::SCALE) * b2Vec2(vec.x, vec.y);
}

static inline glm::vec2 b2n(b2Vec2 vec) {
	return glm::vec2(vec.x, vec.y) * (float)Oort::SCALE;
}

#endif
