#ifndef VECTOR_TYPES_H
#define VECTOR_TYPES_H

typedef union Vec2 {
	struct {
		double x;
		double y;
	};
	double data[2];
} Vec2;

typedef union Vec2f {
	struct {
		float x;
		float y;
	};
	float data[2];
} Vec2f;

typedef union Vec4f {
	struct {
		float x;
		float y;
		float z;
		float w;
	};
	float data[4];
} Vec4f;

typedef union Mat4f {
	float data[16];
} Mat4f;

#endif
