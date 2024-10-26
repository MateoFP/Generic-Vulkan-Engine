#pragma once
#include "game_backend.h"

#define CAM_SPEED 2.5f

BoundingBox world_box
{
	{600.0f, 1.0f, 600.0f},
	{-600.0f, 1.0f, -600.0f}
};
Camera3D cam =
{
	{0.0f, 1.0f,0.0f},
	{-20.0f, 90.0f, 0.0f}
};
mat4 mubo[3];

void init_player(GameInput *input, Character *player)
{
	//player
	player->position = {0.0f, 0.0f, 0.0f};
	player->movement = {0.0f, 0.0f, 0.0f};
	player->destination = {0.0f, 0.0f, 0.0f};
	player->speed = 0.2f;
	player->angle = 0.0f;
	player->moving = false;
	input->MouseRay.position = cam.position;
	//boss1
	mubo[2] = mat4_multiply(mat4_translate({0.0f, 0.0f, 0.0f}), mat4_scale({7.5f, 5.5f, 7.5f}));
}
v3 RayCast(double xpos, double ypos, mat4 view, mat4 projection, float width, float height)
{
	float x = (2.0f * xpos) / width - 1.0f;
	float y = (2.0f * ypos) / height - 1.0f;
	float z = 1.0f;
	v3 ray_nds = {x, y, z};

	v4 ray_clip = {ray_nds.xyz[0], ray_nds.xyz[1], ray_nds.xyz[2], 1.0f};
	mat4 invProjMatrix = mat4_inv_perspective(projection);
	v4 ray_eye = mat4_v4_linear_combine(ray_clip, invProjMatrix);

	ray_eye = {ray_eye.xyzw[0], ray_eye.xyzw[1], ray_eye.xyzw[2], 0.0f};

	mat4 invViewMatrix = mat4_inv_lookat(view);
	v4 inv_ray_wor = mat4_v4_linear_combine(ray_eye, invViewMatrix);

	v3 ray_wor = {inv_ray_wor.xyzw[0], inv_ray_wor.xyzw[1], inv_ray_wor.xyzw[2]};
	ray_wor = v3_normalize(ray_wor);

	return ray_wor;
}
RayCollision GetRayCollisionBox(Ray ray, BoundingBox box)
{
	RayCollision collision = { 0 };

	// Note: If ray.position is inside the box, the distance is negative (as if the ray was reversed)
	// Reversing ray.direction will give use the correct result
	bool insideBox = (ray.position.xyz[0] > box.min.xyz[0]) && (ray.position.xyz[0] < box.max.xyz[0]) &&
		(ray.position.xyz[1] > box.min.xyz[1]) && (ray.position.xyz[1] < box.max.xyz[1]) &&
		(ray.position.xyz[2] > box.min.xyz[2]) && (ray.position.xyz[2] < box.max.xyz[2]);

	if(insideBox) ray.direction = v3_negative(ray.direction);

	float t[11] = { 0 };

	t[8] = 1.0f / ray.direction.xyz[0];
	t[9] = 1.0f / ray.direction.xyz[1];
	t[10] = 1.0f / ray.direction.xyz[2];

	t[0] = (box.min.xyz[0] - ray.position.xyz[0]) * t[8];
	t[1] = (box.max.xyz[0] - ray.position.xyz[0]) * t[8];
	t[2] = (box.min.xyz[1] - ray.position.xyz[1]) * t[9];
	t[3] = (box.max.xyz[1] - ray.position.xyz[1]) * t[9];
	t[4] = (box.min.xyz[2] - ray.position.xyz[2]) * t[10];
	t[5] = (box.max.xyz[2] - ray.position.xyz[2]) * t[10];
	t[6] = (float)fmax(fmax(fmin(t[0], t[1]), fmin(t[2], t[3])), fmin(t[4], t[5]));
	t[7] = (float)fmin(fmin(fmax(t[0], t[1]), fmax(t[2], t[3])), fmax(t[4], t[5]));

	collision.hit = !((t[7] < 0) || (t[6] > t[7]));
	collision.distance = t[6];
	collision.point = v3_add(ray.position, v3_scale(ray.direction, collision.distance));

	// Get box center point
	collision.normal = v3_lerp(box.min, box.max, 0.5f);
	// Get vector center point->hit point
	collision.normal = v3_subtract(collision.point, collision.normal); //MAYBE NT WORK
	// Scale vector to unit cube
	// NOTE: We use an additional .01 to fix numerical errors
	collision.normal = v3_scale(collision.normal, 2.01f);
	collision.normal = v3_divide(collision.normal, v3_subtract(box.max, box.min));
	// The relevant elements of the vector are now slightly larger than 1.0f (or smaller than -1.0f)
	// and the others are somewhere between -1.0 and 1.0 casting to int is exactly our wanted normal!
	collision.normal.xyz[0] = (float)((int)collision.normal.xyz[0]);
	collision.normal.xyz[1] = (float)((int)collision.normal.xyz[1]);
	collision.normal.xyz[2] = (float)((int)collision.normal.xyz[2]);

	collision.normal = v3_normalize(collision.normal);

	if(insideBox)
	{
		// Reset ray.direction
		ray.direction = v3_negative(ray.direction); //MAYBE NOT WORK
		// Fix result
		collision.distance *= -1.0f;
		collision.normal = v3_negative(collision.normal);
	}

	return collision;
}
void update_UBO(Character *player)
{	
	gubo.model = mat4_diagonal(1.0f);
	gubo.view = mat4_look_at(cam.position, cam.look_at, {0.0f,1.0f,0.0f});
	gubo.proj = mat4_perspective(DegToRad(40.0f), (float)extent.width/(float)extent.height, 1.0f, 120.0f);
	gubo.proj.element[1][1] *= -1;
	gubo.projView = mat4_multiply(gubo.proj, gubo.view); //maybe broken
	memcpy(global_uniform_buffer_mapped, &gubo, sizeof(gubo));

	mat4 MatPlayerRotation = mat4_rotate_RH(player->angle, {0.0f, 1.0f, 0.0f});
	mat4 MatPlayerTranslation = mat4_translate({player->position.xyz[0], 0.0f, player->position.xyz[2]});
	mat4 MatPlayerScale = mat4_scale({5.5f, 5.5f, 5.5f});
	mat4 MatPlayer = mat4_multiply(MatPlayerRotation, MatPlayerScale);
	MatPlayer = mat4_multiply(MatPlayerTranslation, MatPlayer);

	mubo[0] = MatPlayer;
	mubo[1] = mat4_multiply(mat4_translate({8.0f, 0.0f, 8.0f}), mat4_scale({9.0f, 9.0f, 9.0f}));
	memcpy(model_uniform_buffer_mapped, &mubo, sizeof(mat4) *MODEL_NUM);
}
void draw_frame()
{
	vkWaitForFences(logical_device, 1, &in_flight_fences, VK_TRUE, UINT64_MAX);

	uint32_t image_index;
	vkAcquireNextImageKHR(logical_device, swapchain, UINT64_MAX, image_available_semaphores, VK_NULL_HANDLE, &image_index);

	vkResetFences(logical_device, 1, &in_flight_fences);

	vkResetCommandBuffer(gcommand_buffer, 0);
	record_gcommand_buffer(gcommand_buffer, image_index);

	VkSemaphore waitSemaphores[] = {image_available_semaphores};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	VkSemaphore signalSemaphores[] = {render_finished_semaphores};

	VkSubmitInfo submitCreateInfo{};
	submitCreateInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitCreateInfo.waitSemaphoreCount = 1;
	submitCreateInfo.pWaitSemaphores = waitSemaphores;
	submitCreateInfo.pWaitDstStageMask = waitStages;
	submitCreateInfo.commandBufferCount = 1;
	submitCreateInfo.pCommandBuffers = &gcommand_buffer;
	submitCreateInfo.signalSemaphoreCount = 1;
	submitCreateInfo.pSignalSemaphores = signalSemaphores;

	if(vkQueueSubmit(graphics_queue, 1, &submitCreateInfo, in_flight_fences) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit draw command buffer.");
	}

	VkSwapchainKHR swapchains[] = {swapchain};

	VkPresentInfoKHR presentCreateInfo{};
	presentCreateInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentCreateInfo.waitSemaphoreCount = 1;
	presentCreateInfo.pWaitSemaphores = signalSemaphores;
	presentCreateInfo.swapchainCount = 1;
	presentCreateInfo.pSwapchains = swapchains;
	presentCreateInfo.pImageIndices = &image_index;
	presentCreateInfo.pResults = nullptr; // Optional

	if(vkQueuePresentKHR(graphics_queue, &presentCreateInfo) != VK_SUCCESS)
	{
		OutputDebugStringA("failed to present\n");
	}
}

void GameUpdate(Character *player, GameInput* input)
{
	if(player->moving)
	{
		player->position.xyz[0] += player->movement.xyz[0];
		player->position.xyz[2] += player->movement.xyz[2];

		if(v2_distance({player->position.xyz[0], player->position.xyz[2]}, 
					   {player->destination.xyz[0], player->destination.xyz[2]}) < 0.20f)
		{
			player->moving = false;
		}
	}

	if(input->mouse_x < 27)
	{
		cam.position.xyz[2] -= CAM_SPEED;
		cam.look_at.xyz[2] -= CAM_SPEED;
	}
	else if(input->mouse_x > 1556)
	{
		cam.position.xyz[2] += CAM_SPEED;
		cam.look_at.xyz[2] += CAM_SPEED;
	}
	if(input->mouse_y < 21)
	{
		cam.position.xyz[0] += CAM_SPEED;
		cam.look_at.xyz[0] += CAM_SPEED;
	}
	else if(input->mouse_y > 833)
	{
		cam.position.xyz[0] -= CAM_SPEED;
		cam.look_at.xyz[0] -= CAM_SPEED;
	}

	if(input->key_spacebar.ended_down)
	{
		cam.position = {player->position.xyz[0] -20.0f, cam.position.xyz[1], player->position.xyz[2]};
		cam.look_at = {player->position.xyz[0], 1.0f, player->position.xyz[2]};
	}
	input->MouseRay = {cam.position};

	draw_frame();
	update_UBO(player);
}