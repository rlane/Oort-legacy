#include "sim/ai_lib.h"
#include "sim/ship.h"
#include "sim/math_util.h"
#include "glm/gtx/vector_angle.hpp"

using namespace std;
using namespace glm;

namespace Oort {
namespace AILib {

float smallest_positive_root_of_quadratic_equation(float a, float b, float c) {
	float z = sqrtf(b*b - 4*a*c);
	float x1 = (b + z)/(2*a);
	float x2 = (b - z)/(2*a);
	if (x1 < 0) {
		return x2;
	} else if (x2 < 0) {
		return x1;
	} else {
		return fminf(x1, x2);
	}
}

int accelerated_goto(float p, float v, float a) {
	auto _a = a;
	auto _b = 2*v;
	auto _c = -p + v*v/(2*a);
	auto t = smallest_positive_root_of_quadratic_equation(_a, _b, _c);
	if (t > 0) {
		return 1;
	} else {
		return -1;
	}
}

void turn_to(Ship &s, float angle) {
	auto h = s.get_heading();
	auto w = s.get_angular_velocity();
	auto wa = 2.0f;
	auto diff = angle_diff(h, angle);
	auto f = accelerated_goto(diff, -w, wa);
	s.acc_angular(f * wa);
}

void drive_towards(Ship &s, vec2 tp, float speed) {
	auto p = s.get_position();
	auto v = s.get_velocity();
	auto h = s.get_heading();
	auto a = radians(orientedAngle(vec2(1,0), normalize(tp-p)));
	auto rv = rotate(v, degrees(-h));
	s.acc_lateral(-1*rv.y);
	turn_to(s, a);

	float max_main_acc = 100;
	auto diff = angle_diff(a,h);
	if (rv.x > speed) {
		s.acc_main(speed-rv.x);
	} else if (fabsf(diff) < M_PI/4) {
		s.acc_main(max_main_acc);
	} else if (fabsf(diff) > 3*M_PI/4) {
		s.acc_main(-max_main_acc);
	} else {
		s.acc_main(0);
	}
}

}
}
