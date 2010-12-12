#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <sys/time.h>
#include <math.h>

#include "physics.h"
#include "vector.h"

#if 0
Vec2 C(double x, double y)
{
	return x + y*I;
}
#endif

#define C(x,y) ((Vec2) { x, y })

typedef struct testcase {
	Vec2 p1, v1;
	double r1;
	Vec2 p2, v2;
	double r2;
	Vec2 cp;
} testcase;

testcase testcases[] = {
	{ C(0.0, 0.0), C(0.0, 0.0), 1.0,
		C(-2.0, 0.0), C(10.0, 0.0), 0.1,
		C(-1.1, 0.0) },

	// no collision
	{ C(0.0, 0.0), C(0.0, 0.0), 1.0,
		C(-2.0, -2.0), C(10.0, 0.0), 0.1,
		C(NAN,NAN) },

	// future collision
	{ C(0.0, 0.0), C(0.0, 0.0), 1.0,
		C(-2.0, 1.0), C(0.1, 0.0), 0.1,
		C(NAN,NAN) },

	// past collision
	{ C(0.0, 0.0), C(0.0, 0.0), 1.0,
		C(2.0, 1.0), C(0.1, 0.0), 0.1,
		C(NAN,NAN) },

	/*
	{ C(0.0, 0.0), C(0.0, 0.0), C(-1.0, 0.1), C(1.0, 0.1), 0.1, 0.1, C(0.0, 0.1) },
	{ C(0.0, 0.0), C(0.0, 0.0), C(0.1, -1.0), C(0.1, 1.0), 0.1, 0.1, C(0.1, 0.0) },

	{ C(-1.0, 0.1), C(1.0, 0.1), C(0.0, 0.0), C(0.0, 0.0), 0.1, 0.1, C(0.0, 0.1) },
	{ C(0.1, -1.0), C(0.1, 1.0), C(0.0, 0.0), C(0.0, 0.0), 0.1, 0.1, C(0.1, 0.0) },

	{ C(0.0, 0.0), C(0.0, 0.0), C(-1.0, 1.0), C(1.0, 1.0), 0.0, 1.0, C(0.0, 1.0) },
	{ C(0.0, 0.0), C(0.0, 0.0), C(1.0, -1.0), C(1.0, 1.0), 0.0, 1.0, C(1.0, 0.0) },

	{ C(0.0, 0.0), C(0.0, 0.0), C(1.0, 0.0), C(1.0, 0.0), 0.0, 0.0, NAN },

	{ C(0.1, -1.0), C(0.1, 1.0), C(0.0, 0.0), C(0.0, 0.0), 0.1, 0.1, C(0.1, 0.0) },
	*/
};

int main(int argc, char **argv)
{
	int i;
	for (i = 0; i < sizeof(testcases)/sizeof(testcase); i++) {
		testcase c = testcases[i];
		Vec2 cp;
		struct physics q1 = { .p = c.p1, .v = c.v1, .r = c.r1 };
		struct physics q2 = { .p = c.p2, .v = c.v2, .r = c.r2 };
		if (!physics_check_collision(&q1, &q2, 1, &cp)) {
			cp.x = NAN;
			cp.y = NAN;
			printf("no collision\n");
		}

		printf("p1=" VEC2_FMT " v1=" VEC2_FMT " r1=%0.2g\n",
				   VEC2_ARG(c.p1), VEC2_ARG(c.v1), c.r1);

		printf("p2=" VEC2_FMT " v2=" VEC2_FMT " r2=%0.2g\n",
				   VEC2_ARG(c.p2), VEC2_ARG(c.v2), c.r2);

		printf("got=" VEC2_FMT " expected=" VEC2_FMT " ",
				   VEC2_ARG(cp), VEC2_ARG(c.cp));

		if ((cp.x == c.cp.x && cp.y == c.cp.y) || (isnan(cp.x) && isnan(c.cp.x))) {
			printf("pass\n");
		} else {
			printf("fail\n");
		}

		printf("\n");
	}
	
	return 0;
}
