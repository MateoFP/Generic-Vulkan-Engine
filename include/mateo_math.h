#pragma once

#include <math.h>
#define PI 3.14159265358979323846

static inline float DegToRad(float degrees)
{
	return (degrees * (PI/180.0f));
}
static inline float InvSqrtF(float Float)
{
	float result;
	result = 1.0f / sqrtf(Float);
	return result;
}

typedef struct v2
{
	float xy[2];
} v2;
typedef struct v3
{
	float xyz[3];
} v3;
typedef struct v4
{
	float xyzw[4];
} v4;
typedef union mat3
{
	float element[3][3];
	v3 column[3];
} mat3;
typedef union mat4
{
	float element[4][4];
	v4 column[4];
} mat4;

static inline float v2_distance(v2 left, v2 right)
{
	float result = 0.0f;

	float dx = left.xy[0] - right.xy[0];
	float dy = left.xy[1] - right.xy[1];
	result = sqrtf(dx * dx + dy * dy);

	return result;
}
static inline v2 v2_normalize(v2 vector)
{
	v2 result = vector;

	float length = sqrtf(vector.xy[0] * vector.xy[0] + vector.xy[1] * vector.xy[1]);
	if(length != 0.0f)
	{
		float ilength = 1.0f / length;

		result.xy[0] *= ilength;
		result.xy[1] *= ilength;
	}

	return result;
}

static inline float v3_distance(v3 left, v3 right)
{
	float result = 0.0f;

	float dx = left.xyz[0] - right.xyz[0];
	float dy = left.xyz[1] - right.xyz[1];
	float dz = left.xyz[2] - right.xyz[2];
	result = sqrtf(dx * dx + dy * dy + dz * dz);

	return result;
}
static inline float v3_dot(v3 left, v3 right)
{
	return (left.xyz[0] * right.xyz[0]) + (left.xyz[1] * right.xyz[1]) + (left.xyz[2] * right.xyz[2]);
}
static inline v3 v3_multiplyf(v3 vector, float Float)
{
	v3 result;

	result.xyz[0] = vector.xyz[0] * Float;
	result.xyz[1] = vector.xyz[1] * Float;
	result.xyz[2] = vector.xyz[2] * Float;

	return result;
}
static inline v3 v3_cross(v3 left, v3 right)
{
	v3 result;

	result.xyz[0] = (left.xyz[1] * right.xyz[2]) - (left.xyz[2] * right.xyz[1]);
	result.xyz[1] = (left.xyz[2] * right.xyz[0]) - (left.xyz[0] * right.xyz[2]);
	result.xyz[2] = (left.xyz[0] * right.xyz[1]) - (left.xyz[1] * right.xyz[0]);

	return result;
}
static inline v3 v3_normalize(v3 vector)
{
	return v3_multiplyf(vector, InvSqrtF(v3_dot(vector, vector)));
}
static inline v3 v3_lerp(v3 left, v3 right, float amount)
{
	v3 result = {};

	result.xyz[0] = left.xyz[0] + amount * (right.xyz[0] - left.xyz[0]);
	result.xyz[1] = left.xyz[1] + amount * (right.xyz[1] - left.xyz[1]);
	result.xyz[2] = left.xyz[2] + amount * (right.xyz[2] - left.xyz[2]);

	return result;
}
static inline v3 v3_scale(v3 vector, float scalar)
{
	v3 result = {vector.xyz[0] * scalar, vector.xyz[1] * scalar, vector.xyz[2] * scalar};

	return result;
}
static inline v3 v3_negative(v3 vector)
{
	v3 result = {-vector.xyz[0], -vector.xyz[1], -vector.xyz[2]};

	return result;
}
static inline v3 v3_add(v3 left, v3 right)
{
	v3 result = {left.xyz[0] + right.xyz[0], left.xyz[1] + right.xyz[1], left.xyz[2] + right.xyz[2]};

	return result;
}
static inline v3 v3_subtract(v3 left, v3 right)
{
	v3 result;

	result.xyz[0] = left.xyz[0] - right.xyz[0];
	result.xyz[1] = left.xyz[1] - right.xyz[1];
	result.xyz[2] = left.xyz[2] - right.xyz[2];

	return result;
}
static inline v3 v3_divide(v3 left, v3 right)
{
	v3 result = {left.xyz[0] / right.xyz[0], left.xyz[1] / right.xyz[1], left.xyz[2] / right.xyz[2]};

	return result;
}

static inline mat3 mat3_transpose(mat3 matrix)
{
	mat3 result = matrix;

	result.element[0][1] = matrix.element[1][0];
	result.element[0][2] = matrix.element[2][0];
	result.element[1][0] = matrix.element[0][1];
	result.element[1][2] = matrix.element[2][1];
	result.element[2][1] = matrix.element[1][2];
	result.element[2][0] = matrix.element[0][2];

	return result;
}

static inline v4   mat4_v4_linear_combine(v4 vector, mat4 matrix)
{
	v4 result;

	result.xyzw[0] = vector.xyzw[0] * matrix.element[0][0];
	result.xyzw[1] = vector.xyzw[0] * matrix.element[0][1];
	result.xyzw[2] = vector.xyzw[0] * matrix.element[0][2];
	result.xyzw[3] = vector.xyzw[0] * matrix.element[0][3];

	result.xyzw[0] += vector.xyzw[1] * matrix.element[1][0];
	result.xyzw[1] += vector.xyzw[1] * matrix.element[1][1];
	result.xyzw[2] += vector.xyzw[1] * matrix.element[1][2];
	result.xyzw[3] += vector.xyzw[1] * matrix.element[1][3];

	result.xyzw[0] += vector.xyzw[2] * matrix.element[2][0];
	result.xyzw[1] += vector.xyzw[2] * matrix.element[2][1];
	result.xyzw[2] += vector.xyzw[2] * matrix.element[2][2];
	result.xyzw[3] += vector.xyzw[2] * matrix.element[2][3];

	result.xyzw[0] += vector.xyzw[3] * matrix.element[3][0];
	result.xyzw[1] += vector.xyzw[3] * matrix.element[3][1];
	result.xyzw[2] += vector.xyzw[3] * matrix.element[3][2];
	result.xyzw[3] += vector.xyzw[3] * matrix.element[3][3];

	return result;
}
static inline mat4 mat4_diagonal(float diagonal)
{
	mat4 result = {0};

	result.element[0][0] = diagonal;
	result.element[1][1] = diagonal;
	result.element[2][2] = diagonal;
	result.element[3][3] = diagonal;

	return result;
}
static inline mat4 mat4_rotate_RH(float angle, v3 axis)
{
	mat4 result = mat4_diagonal(1.0f);

	axis = v3_normalize(axis);

	float SinTheta = sinf(angle);
	float CosTheta = cosf(angle);
	float CosValue = 1.0f - CosTheta;

	result.element[0][0] = (axis.xyz[0] * axis.xyz[0]) + CosTheta;
	result.element[0][1] = (axis.xyz[0] * axis.xyz[1] * CosValue) + (axis.xyz[2] * SinTheta);
	result.element[0][2] = (axis.xyz[0] * axis.xyz[2] * CosValue) - (axis.xyz[1] * SinTheta);

	result.element[1][0] = (axis.xyz[1] * axis.xyz[0] * CosValue) - (axis.xyz[2] * SinTheta);
	result.element[1][1] = (axis.xyz[1] * axis.xyz[1] * CosValue) + CosTheta;
	result.element[1][2] = (axis.xyz[1] * axis.xyz[2] * CosValue) + (axis.xyz[0] * SinTheta);

	result.element[2][0] = (axis.xyz[2] * axis.xyz[0] * CosValue) + (axis.xyz[1] * SinTheta);
	result.element[2][1] = (axis.xyz[2] * axis.xyz[1] * CosValue) - (axis.xyz[0] * SinTheta);
	result.element[2][2] = (axis.xyz[2] * axis.xyz[2] * CosValue) + CosTheta;

	return result;
}
static inline mat4 mat4_translate(v3 vector)
{
	mat4 result = mat4_diagonal(1.0f);

	result.element[3][0] = vector.xyz[0];
	result.element[3][1] = vector.xyz[1];
	result.element[3][2] = vector.xyz[2];

	return result;
}
static inline mat4 mat4_scale(v3 vector)
{
	mat4 result = mat4_diagonal(1.0f);

	result.element[0][0] = vector.xyz[0];
	result.element[1][1] = vector.xyz[1];
	result.element[2][2] = vector.xyz[2];
	
	return result;
}
static inline mat4 mat4_multiply(mat4 left, mat4 right)
{
	mat4 result;

	result.column[0] = mat4_v4_linear_combine(right.column[0], left);
	result.column[1] = mat4_v4_linear_combine(right.column[1], left);
	result.column[2] = mat4_v4_linear_combine(right.column[2], left);
	result.column[3] = mat4_v4_linear_combine(right.column[3], left);

	return result;
}
static inline mat4 mat4_perspective(float FOV, float aspect_ratio, float Near, float Far)
{
	mat4 result = {};

	float cotangent = 1.0f / tanf(FOV / 2.0f);
	result.element[0][0] = cotangent / aspect_ratio;
	result.element[1][1] = cotangent;
	result.element[2][3] = -1.0f;

	result.element[2][2] = (Near + Far) / (Near - Far);
	result.element[3][2] = (2.0f * Near * Far) / (Near - Far);

	return result;
}
static inline mat4 mat4_look_at(v3 eye, v3  center, v3  up)
{
	v3 F = v3_normalize(v3_subtract(center, eye));
	v3 S = v3_normalize(v3_cross(F, up));
	v3 U = v3_cross(S, F);

	mat4 result;

	result.element[0][0] = S.xyz[0];
	result.element[0][1] = U.xyz[0];
	result.element[0][2] = -F.xyz[0];
	result.element[0][3] = 0.0f;

	result.element[1][0] = S.xyz[1];
	result.element[1][1] = U.xyz[1];
	result.element[1][2] = -F.xyz[1];
	result.element[1][3] = 0.0f;

	result.element[2][0] = S.xyz[2];
	result.element[2][1] = U.xyz[2];
	result.element[2][2] = -F.xyz[2];
	result.element[2][3] = 0.0f;

	result.element[3][0] = -v3_dot(S, eye);
	result.element[3][1] = -v3_dot(U, eye);
	result.element[3][2] = v3_dot(F, eye);
	result.element[3][3] = 1.0f;

	return result;
}
static inline mat4 mat4_inv_perspective(mat4 matrix)
{
	mat4 result = {0};
	result.element[0][0] = 1.0f / matrix.element[0][0];
	result.element[1][1] = 1.0f / matrix.element[1][1];
	result.element[2][2] = 0.0f;

	result.element[2][3] = 1.0f / matrix.element[3][2];
	result.element[3][3] = matrix.element[2][2] * result.element[2][3];
	result.element[3][2] = matrix.element[2][3];

	return result;
}
static inline mat4 mat4_inv_lookat(mat4 matrix)
{
	mat4 Result;

	mat3 rotation = {0};
	rotation.column[0] = {matrix.column[0].xyzw[0], matrix.column[0].xyzw[1], matrix.column[0].xyzw[2]};
	rotation.column[1] = {matrix.column[1].xyzw[0], matrix.column[1].xyzw[1], matrix.column[1].xyzw[2]};
	rotation.column[2] = {matrix.column[2].xyzw[0], matrix.column[2].xyzw[1], matrix.column[2].xyzw[2]};
	rotation = mat3_transpose(rotation);

	Result.column[0] = {rotation.element[0][0], rotation.element[0][1], rotation.element[0][2], 0.0f};
	Result.column[1] = {rotation.element[1][0], rotation.element[1][1], rotation.element[1][2], 0.0f};
	Result.column[2] = {rotation.element[2][0], rotation.element[2][1], rotation.element[2][2], 0.0f};
	Result.column[3] = {matrix.element[3][0] * -1.0f, matrix.element[3][1] * -1.0f, 
						matrix.element[3][2] * -1.0f, matrix.element[3][3] * -1.0f};

	Result.element[3][0] = -1.0f * matrix.element[3][0] / 
												(rotation.element[0][0] + rotation.element[0][1] + rotation.element[0][2]);
	Result.element[3][1] = -1.0f * matrix.element[3][1] /
												(rotation.element[1][0] + rotation.element[1][1] + rotation.element[1][2]);
	Result.element[3][2] = -1.0f * matrix.element[3][2] /
												(rotation.element[2][0] + rotation.element[2][1] + rotation.element[2][2]);
	Result.element[3][3] = 1.0f;

	return Result;
}