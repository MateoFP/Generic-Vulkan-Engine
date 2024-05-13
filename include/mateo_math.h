#pragma once

#include <math.h>
#define PI 3.14159265358979323846

static inline float DegToRad(float Float)
{
	return Float * (PI/180.0f);
}

static inline float InvSqrtF(float Float)
{
	float result;
	result = 1.0f / sqrtf(Float);
	return result;
}

typedef struct vec2
{
	float element[2];
} vec2;

typedef struct vec3
{
	float element[3];
} vec3;

typedef struct vec4
{
	float element[4];
} vec4;

typedef struct vertex
{
	vec2 pos;
	vec3 color;
} vertex;

typedef struct mat4
{
	vec4 vec[4];
} mat4;

static inline mat4 mat4_diagonal(float diagonal)
{
	mat4 result = {0};
	result.vec[0].element[0] = diagonal;
	result.vec[1].element[1] = diagonal;
	result.vec[2].element[2] = diagonal;
	result.vec[3].element[3] = diagonal;
	return result;
}

static inline vec3 vec3_subtract(vec3 left, vec3 right)
{
	vec3 result;
	result.element[0] = left.element[0] - right.element[0];
	result.element[1] = left.element[1] - right.element[1];
	result.element[2] = left.element[2] - right.element[2];
	return result;
}

static inline vec3 vec3_multiplyf(vec3 left, float right)
{
	vec3 result;
	result.element[0] = left.element[0] * right;
	result.element[1] = left.element[1] * right;
	result.element[2] = left.element[2] * right;
	return result;
}

static inline float vec3_dot(vec3 left, vec3 right)
{
	return (left.element[0] * right.element[0]) + (left.element[1] * right.element[1]) + (left.element[2] * right.element[2]);
}

static inline vec3 vec3_cross(vec3 left, vec3 right)
{
	vec3 Result;
	Result.element[0] = (left.element[1] * right.element[2]) - (left.element[2] * right.element[1]);
	Result.element[1] = (left.element[2] * right.element[0]) - (left.element[0] * right.element[2]);
	Result.element[2] = (left.element[0] * right.element[1]) - (left.element[1] * right.element[0]);
	return Result;
}

static inline vec3 vec3_normalize(vec3 vector)
{
	return vec3_multiplyf(vector, InvSqrtF(vec3_dot(vector, vector)));
}

static inline mat4 mat4_rotate(mat4 matrix, float angle, vec3 axis)
{
	matrix = mat4_diagonal(1.0f);
	axis = vec3_normalize(axis);

	float SinTheta = sinf(angle);
	float CosTheta = cosf(angle);
	float CosValue = 1.0f - CosTheta;

	matrix.vec[0].element[0] = (axis.element[0] * axis.element[0] * CosValue) + CosTheta;
	matrix.vec[0].element[1] = (axis.element[0] * axis.element[1] * CosValue) + (axis.element[2] * SinTheta);
	matrix.vec[0].element[2] = (axis.element[0] * axis.element[2] * CosValue) - (axis.element[1] * SinTheta);

	matrix.vec[1].element[0] = (axis.element[1] * axis.element[0] * CosValue) - (axis.element[2] * SinTheta);
	matrix.vec[1].element[1] = (axis.element[1] * axis.element[1] * CosValue) + CosTheta;
	matrix.vec[1].element[2] = (axis.element[1] * axis.element[2] * CosValue) + (axis.element[0] * SinTheta);

	matrix.vec[2].element[0] = (axis.element[2] * axis.element[0] * CosValue) + (axis.element[1] * SinTheta);
	matrix.vec[2].element[1] = (axis.element[2] * axis.element[1] * CosValue) - (axis.element[0] * SinTheta);
	matrix.vec[2].element[2] = (axis.element[2] * axis.element[2] * CosValue) + CosTheta;

	return matrix;
}

static inline mat4 mat4_lookat(vec3 F, vec3 S, vec3 U, vec3 Eye)
{
	mat4 result;

	result.vec[0].element[0] = S.element[0];
	result.vec[0].element[1] = U.element[0];
	result.vec[0].element[2] = -F.element[0];
	result.vec[0].element[3] = 0.0f;

	result.vec[1].element[0] = S.element[1];
	result.vec[1].element[1] = U.element[1];
	result.vec[1].element[2] = -F.element[1];
	result.vec[1].element[3] = 0.0f;

	result.vec[2].element[0] = S.element[2];
	result.vec[2].element[1] = U.element[2];
	result.vec[2].element[2] = -F.element[2];
	result.vec[2].element[3] = 0.0f;

	result.vec[3].element[0] = -vec3_dot(S, Eye);
	result.vec[3].element[1] = -vec3_dot(U, Eye);
	result.vec[3].element[2] = vec3_dot(F, Eye);
	result.vec[3].element[3] = 1.0f;

	return result;
}

static inline mat4 mat4_lookat_RH(vec3 Eye, vec3 Center, vec3 Up)
{
	vec3 F = vec3_normalize(vec3_subtract(Center, Eye));
	vec3 S = vec3_normalize(vec3_cross(F, Up));
	vec3 U = vec3_cross(S, F);

	return mat4_lookat(F, S, U, Eye);
}

static inline mat4 mat4_perspective(float FOV, float AspectRatio, float Near, float Far)
{
	mat4 result = {0};

	float Cotangent = 1.0f / tanf(FOV / 2.0f);
	result.vec[0].element[0] = Cotangent / AspectRatio;
	result.vec[1].element[1] = Cotangent;
	result.vec[2].element[3] = -1.0f;

	result.vec[2].element[2] = (Near + Far) / (Near - Far);
	result.vec[3].element[2] = (2.0f * Near * Far) / (Near - Far);

	return result;
}