#pragma once
#include "init_vulkan.h"

typedef struct Camera3D
{
	v3 look_at;
	v3 position;
} Camera3D;

typedef struct BoundingBox
{
	v3 max;
	v3 min;
} BoundingBox;

typedef struct Ray
{
	v3 position;
	v3 direction;
} Ray;

typedef struct RayCollision
{
	bool hit;
	float distance;
	v3 point;
	v3 normal;
} RayCollision;

typedef struct GameButtonState
{
	uint32_t	half_transition_count;
	bool		ended_down;
} GameButtonState;

typedef struct GameInput
{
	int32_t mouse_x, mouse_y, mouse_z;
	Ray MouseRay;
	RayCollision MouseRayCollision;

	union
	{
		GameButtonState buttons[6];
		struct
		{
			GameButtonState key_a;
			GameButtonState key_q;
			GameButtonState key_e;
			GameButtonState key_spacebar;
			GameButtonState mouse_rb;
			GameButtonState mouse_lb;
		};
	};
} GameInput;

typedef struct Character
{
	v3 position;
	v3 movement;
	v3 destination;
	float speed;
	float angle;
	bool moving;
}Character;
