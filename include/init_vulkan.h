#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define STB_IMAGE_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define TINYOBJLOADER_IMPLEMENTATION

#define WIDTH 1600
#define HEIGHT 900	

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <stb_image.h>
#include <tiny_obj_loader.h>
#include "mateo_math.h"

typedef struct Vertex 
{
	vec3 pos;
	vec3 color;
	vec2 tex_coords;

	static VkVertexInputBindingDescription getBindingDescription() 
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static VkVertexInputAttributeDescription* getAttributeDescriptions() 
	{
		VkVertexInputAttributeDescription* attributeDescriptions = {};
		attributeDescriptions = (VkVertexInputAttributeDescription*)malloc(3 * sizeof(VkVertexInputAttributeDescription));

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, tex_coords);

		return attributeDescriptions;
	}
} Vertex;

typedef struct UBO
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
} UBO;

typedef struct ReadEntireFile
{
	char*	contents;
	size_t	contents_size;
} ReadEntireFile;

typedef struct Image
{
	VkImage			image;
	VkImageView		view;
	VkDeviceMemory	memory;
	VkFormat		format;
	int				width;
	int				height;
	int				depth;
	int				channels;
} Image;


const char* room_path		     = "C:/dev/myVulkan-1/resources/models/room.obj";
const char* model_path			 = "C:/dev/myVulkan-1/resources/models/model.obj";
const char* furnace_tex_path	 = "C:/dev/myVulkan-1/resources/textures/furnace_tex.png";
const char* wood_tex_path		 = "C:/dev/myVulkan-1/resources/textures/wood_tex.png";
const char* frag_file			 = "C:/dev/myVulkan-1/resources/shaders/frag_file.spv";
const char* vert_file			 = "C:/dev/myVulkan-1/resources/shaders/vert_file.spv";
const char* instanceLayers[]	 = { "VK_LAYER_KHRONOS_validation" };
const char* instanceExtensions[] = { VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
									 VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
									 VK_KHR_SURFACE_EXTENSION_NAME,
									 VK_KHR_WIN32_SURFACE_EXTENSION_NAME};


std::vector<Vertex>			global_model_vertices;
std::vector<uint32_t>		global_model_indices;

uint32_t					image_idx = 0;
uint32_t					image_idx2 = 1;

uint32_t					image_index;
Image						depth_image{};
Image						image_one{};
Image						image_two{};
VkSampler					image_sampler;

VkBuffer					uniformBuffer;
VkDeviceMemory				uniformBufferMemory;
void*						uniformBufferMapped;

VkDescriptorSet				UBO_descriptor_set;
VkDescriptorSet				tex_descriptor_set;

VkDescriptorPool			descriptor_pool;

VkDescriptorPool			UBO_descriptor_pool;
VkDescriptorPool			tex_descriptor_pool;

VkDescriptorSetLayout		tex_descriptor_set_layout;
VkDescriptorSetLayout		UBO_descriptor_set_layout;

VkDeviceMemory				indexBufferMemory;
VkBuffer					indexBuffer;
VkDeviceMemory				vertexBufferMemory;
VkBuffer					vertexBuffer;

VkSemaphore					image_available_semaphores;
VkSemaphore					render_finished_semaphores;
VkFence						in_flight_fences;

VkCommandBuffer				command_buffer;
VkCommandPool				command_pool;

VkPipeline					graphicsPipeline;
VkPipelineLayout			pipelineLayout;
VkShaderModule				vertShaderModule;
VkShaderModule				fragShaderModule;


VkSwapchainKHR				swapchain;
VkImageView*				swapchain_image_views;
VkImage*					swapchain_images;
uint32_t					image_count;
VkExtent2D					extent;

VkSurfaceFormatKHR			surface_format;
VkPresentModeKHR			surface_present_mode;
VkSurfaceCapabilitiesKHR	surface_cap;
VkSurfaceKHR				surface;

VkDevice					logical_device;
VkQueue						graphics_queue;
uint32_t					graphics_family;

VkQueueFamilyProperties*	queue_family_prop;
uint32_t					queue_family_count;
VkPhysicalDeviceFeatures	physical_device_feat;
VkPhysicalDeviceProperties	physical_device_prop;
VkPhysicalDevice			physical_device;
VkInstance					my_instance;

float num = 1.0f;

VkCommandBuffer begin_single_command_buffer()
{
	VkCommandBufferAllocateInfo allocCreateInfo{};
	allocCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocCreateInfo.commandPool = command_pool;
	allocCreateInfo.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(logical_device, &allocCreateInfo, &command_buffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(command_buffer, &beginInfo);

	return command_buffer;
}
void end_single_command_buffer(VkCommandBuffer command_buffer)
{
	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &command_buffer;

	vkQueueSubmit(graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphics_queue);
	vkFreeCommandBuffers(logical_device, command_pool, 1, &command_buffer);
}

void create_image(Image* image, VkImageTiling tiling, VkImageUsageFlags usage, 
					VkMemoryPropertyFlags prop, VkImageAspectFlags flags)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = (uint32_t)(image->width);
	imageInfo.extent.height = (uint32_t)(image->height);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = image->format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;

	if(vkCreateImage(logical_device, &imageInfo, nullptr, &image->image) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create image.");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(logical_device, image->image, &memRequirements);

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memProperties);

	uint32_t mem_property = 0;
	VkMemoryPropertyFlags properties_flags = prop;
	for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if((memRequirements.memoryTypeBits & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties_flags) == properties_flags)
		{
			mem_property = i;
			break;
		}
	}

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = mem_property;

	if(vkAllocateMemory(logical_device, &allocInfo, nullptr, &image->memory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate image memory.");
	}
	if(vkBindImageMemory(logical_device, image->image, image->memory, 0) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to bind image.image to image.memory.");
	}

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image->image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = image->format;
	viewInfo.subresourceRange.aspectMask = flags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if(vkCreateImageView(logical_device, &viewInfo, nullptr, &image->view) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create texture image view.");
	}
}
void set_image_layout(VkCommandBuffer command_buffer, VkImage images, 
						VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.image = images;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	else if(oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
	{
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = 0;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = 0;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else if(oldLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(command_buffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);
}
void copy_buffer_to_image(VkCommandBuffer command_buffer, VkBuffer buffer, Image* image)
{
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { (uint32_t)image->width, (uint32_t)image->height, 1 };

	vkCmdCopyBufferToImage(command_buffer, buffer, image->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

ReadEntireFile read_entire_file(const char* file_path)
{
	ReadEntireFile ref = {};

	FILE* file = fopen(file_path, "rb");
	if(file == NULL)
	{
		printf("Failed to load file %s, \n", file_path);
		return ref;
	}

	fseek(file, 0, SEEK_END);
	ref.contents_size = (size_t)ftell(file);
	fseek(file, 0, SEEK_SET);

	ref.contents = (char*)malloc(ref.contents_size);
	fread(ref.contents, 1, ref.contents_size, file);
	fclose(file);

	return ref;
}
void update_UBO() 
{
	num++;
	UBO ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), (num/7000.0f) * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.view = glm::lookAt(glm::vec3(5.0f, 15.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 100.0f);
	ubo.proj[1][1] *= -1;

	memcpy(uniformBufferMapped, &ubo, sizeof(ubo));
}

void copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer command_buffer = begin_single_command_buffer();

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(command_buffer, srcBuffer, dstBuffer, 1, &copyRegion);

	end_single_command_buffer(command_buffer);
}
void create_buffer(VkDeviceSize	size, VkBufferUsageFlags usage, VkMemoryPropertyFlags prop,
					VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if(vkCreateBuffer(logical_device, &bufferCreateInfo, 0, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create buffer.");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logical_device, buffer, &memRequirements);

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memProperties);

	uint32_t mem_property = 0;
	VkMemoryPropertyFlags properties_flags = prop;
	for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties_flags) == properties_flags)
		{
			mem_property = i;
			break;
		}
	}

	VkMemoryAllocateInfo memoryAllocInfo{};
	memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocInfo.allocationSize = memRequirements.size;
	memoryAllocInfo.memoryTypeIndex = mem_property;

	if(vkAllocateMemory(logical_device, &memoryAllocInfo, 0, &bufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate buffer memory.");
	}

	vkBindBufferMemory(logical_device, buffer, bufferMemory, 0);
}

void record_command_buffer(VkCommandBuffer command_buffer, uint32_t image_index)
{
	VkCommandBufferBeginInfo beingInfo{};
	beingInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if(vkBeginCommandBuffer(command_buffer, &beingInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin recording command buffer.");
	}

	set_image_layout(command_buffer, swapchain_images[image_index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	VkClearValue clear_value = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

	VkRenderingAttachmentInfo color_attachment_info = {};
	color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
	color_attachment_info.imageView = swapchain_image_views[image_index];
	color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
	color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment_info.clearValue = clear_value;

	VkRenderingAttachmentInfo depth_attachment_info = {};
	depth_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
	depth_attachment_info.imageView = depth_image.view;
	depth_attachment_info.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
	depth_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depth_attachment_info.clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};
	depth_attachment_info.clearValue.depthStencil = {1.0f, 0};

	VkRenderingInfo render_info = {};
	render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
	render_info.renderArea.offset = {0};
	render_info.renderArea.extent = extent;
	render_info.layerCount = 1;
	render_info.colorAttachmentCount = 1;
	render_info.pColorAttachments = &color_attachment_info;
	render_info.pDepthAttachment = &depth_attachment_info;

	VkDescriptorSet sets[2] = {UBO_descriptor_set, tex_descriptor_set};

	vkCmdBeginRendering(command_buffer, &render_info);

		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		VkBuffer vertexBuffers[] = {vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(command_buffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(command_buffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 
								0, 2, sets, 0, nullptr);

		vkCmdPushConstants(command_buffer, pipelineLayout, VK_SHADER_STAGE_ALL,
						   0, sizeof(uint32_t), (void*)&image_idx);

		vkCmdDrawIndexed(command_buffer, global_model_indices.size() -2448, 1, 0, 2448,0);

		vkCmdPushConstants(command_buffer, pipelineLayout, VK_SHADER_STAGE_ALL,
						   0, sizeof(uint32_t), (void*)&image_idx2);

		vkCmdDrawIndexed(command_buffer, 2448, 1, 0, 0, 0);

	vkCmdEndRendering(command_buffer);

	set_image_layout(command_buffer, swapchain_images[image_index], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	if(vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to record command buffer.");
	}
}
void draw_frame()
{
	vkWaitForFences(logical_device, 1, &in_flight_fences, VK_TRUE, UINT64_MAX);

	vkAcquireNextImageKHR(logical_device, swapchain, UINT64_MAX, image_available_semaphores, VK_NULL_HANDLE, &image_index);

	update_UBO();

	vkResetFences(logical_device, 1, &in_flight_fences);

	vkResetCommandBuffer(command_buffer, 0);
	record_command_buffer(command_buffer, image_index);

	VkSemaphore waitSemaphores[] = { image_available_semaphores };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signalSemaphores[] = { render_finished_semaphores };

	VkSubmitInfo submitCreateInfo{};
	submitCreateInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitCreateInfo.waitSemaphoreCount = 1;
	submitCreateInfo.pWaitSemaphores = waitSemaphores;
	submitCreateInfo.pWaitDstStageMask = waitStages;
	submitCreateInfo.commandBufferCount = 1;
	submitCreateInfo.pCommandBuffers = &command_buffer;
	submitCreateInfo.signalSemaphoreCount = 1;
	submitCreateInfo.pSignalSemaphores = signalSemaphores;

	if(vkQueueSubmit(graphics_queue, 1, &submitCreateInfo, in_flight_fences) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit draw command buffer.");
	}

	VkSwapchainKHR swapchains[] = { swapchain };

	VkPresentInfoKHR presentCreateInfo{};
	presentCreateInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentCreateInfo.waitSemaphoreCount = 1;
	presentCreateInfo.pWaitSemaphores = signalSemaphores;
	presentCreateInfo.swapchainCount = 1;
	presentCreateInfo.pSwapchains = swapchains;
	presentCreateInfo.pImageIndices = &image_index;
	presentCreateInfo.pResults = nullptr; // Optional

	vkQueuePresentKHR(graphics_queue, &presentCreateInfo);
}

void create_instance()
{
	VkApplicationInfo appCreateInfo = {};
	appCreateInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appCreateInfo.pApplicationName = "Hello Triangle";
	appCreateInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appCreateInfo.pEngineName = "No Engine";
	appCreateInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appCreateInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appCreateInfo;
	instanceCreateInfo.enabledExtensionCount = sizeof(instanceExtensions) / sizeof(instanceExtensions[0]);
	instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions;
	instanceCreateInfo.enabledLayerCount = 1;
	instanceCreateInfo.ppEnabledLayerNames = instanceLayers;

	if(vkCreateInstance(&instanceCreateInfo, 0, &my_instance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create instance.");
	}
}
void create_surface(HWND win32_handle)
{
	VkWin32SurfaceCreateInfoKHR surface_create_info = {};
	surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surface_create_info.hinstance = GetModuleHandleW(NULL);
	surface_create_info.hwnd = win32_handle;

	if(vkCreateWin32SurfaceKHR(my_instance, &surface_create_info, NULL, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create win32 surface.");
	}
}
void create_logical_device()
{
	//pick physical device
	uint32_t deviceCount = 1;
	queue_family_count = 0;

	vkEnumeratePhysicalDevices(my_instance, &deviceCount, &physical_device);
	vkGetPhysicalDeviceProperties(physical_device, &physical_device_prop);
	vkGetPhysicalDeviceFeatures(physical_device, &physical_device_feat);

	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, 0);
	queue_family_prop = (VkQueueFamilyProperties*)malloc(queue_family_count * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_prop);

	VkBool32 presentSupport = false;
	for(uint32_t i = 0; i < queue_family_count; i++)
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &presentSupport);
		if(presentSupport && (queue_family_prop[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			graphics_family = i;
			break;
		}
	}

	//create logical device
	const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME};
	VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feat = {};
	dynamic_rendering_feat.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
	dynamic_rendering_feat.dynamicRendering = VK_TRUE;

	float queuePriority = 1.0f;

	VkDeviceQueueCreateInfo deviceQueueInfo = {};
	deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueInfo.queueFamilyIndex = graphics_family;
	deviceQueueInfo.queueCount = 1;
	deviceQueueInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pQueueCreateInfos = &deviceQueueInfo;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pEnabledFeatures = &physical_device_feat;
	deviceInfo.pNext = &dynamic_rendering_feat;
	deviceInfo.enabledLayerCount = 1;
	deviceInfo.ppEnabledLayerNames = instanceLayers;
	deviceInfo.enabledExtensionCount = sizeof(deviceExtensions) / sizeof(deviceExtensions[0]);
	deviceInfo.ppEnabledExtensionNames = deviceExtensions;

	if(vkCreateDevice(physical_device, &deviceInfo, 0, &logical_device) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device.");
	}
	vkGetDeviceQueue(logical_device, graphics_family, 0, &graphics_queue);
}
void create_swapchain()
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_cap);

	uint32_t format_count;
	uint32_t present_mode_count;

	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, 0);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, 0);

	VkSurfaceFormatKHR* surface_formats = (VkSurfaceFormatKHR*)malloc(format_count * sizeof(VkSurfaceFormatKHR));
	VkPresentModeKHR* surface_present_modes = (VkPresentModeKHR*)malloc(present_mode_count * sizeof(VkPresentModeKHR));

	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, surface_formats);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, surface_present_modes);

	surface_format = surface_formats[2];
	surface_present_mode = surface_present_modes[0];
	extent = surface_cap.currentExtent;

	image_count = surface_cap.minImageCount + 1;
	if(surface_cap.maxImageCount > 0 && image_count > surface_cap.maxImageCount)
	{
		image_count = surface_cap.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapchainCreateInfo{};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = image_count;
	swapchainCreateInfo.imageFormat = surface_format.format;
	swapchainCreateInfo.imageColorSpace = surface_format.colorSpace;
	swapchainCreateInfo.imageExtent = extent;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCreateInfo.preTransform = surface_cap.currentTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = surface_present_mode;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	if(vkCreateSwapchainKHR(logical_device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swapchain.");
	}

	vkGetSwapchainImagesKHR(logical_device, swapchain, &image_count, nullptr);
	swapchain_images = (VkImage*)malloc(image_count * sizeof(VkImage));
	vkGetSwapchainImagesKHR(logical_device, swapchain, &image_count, swapchain_images);
	swapchain_image_views = (VkImageView*)malloc(image_count * sizeof(VkImageView));

	for(uint32_t i = 0; i < image_count; i++)
	{
		VkImageViewCreateInfo ImageViewCreateInfo{};
		ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.image = swapchain_images[i];
		ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.format = surface_format.format;
		ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ImageViewCreateInfo.subresourceRange.levelCount = 1;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewCreateInfo.subresourceRange.layerCount = 1;

		if(vkCreateImageView(logical_device, &ImageViewCreateInfo, 0, &swapchain_image_views[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image views.");
		}
	}
}
void create_UBO_descriptor_set_layout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if(vkCreateDescriptorSetLayout(logical_device, &layoutInfo, nullptr, &UBO_descriptor_set_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor set layout.");
	}
}
void create_tex_descriptor_set_layout()
{
    VkDescriptorSetLayoutBinding global_texture_bindings[2] = {};
	global_texture_bindings[0].binding = 0;
	global_texture_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	global_texture_bindings[0].descriptorCount = 1;
	global_texture_bindings[0].stageFlags = VK_SHADER_STAGE_ALL;

	global_texture_bindings[1].binding = 1;
	global_texture_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	global_texture_bindings[1].descriptorCount = 2;
	global_texture_bindings[1].stageFlags = VK_SHADER_STAGE_ALL;

    VkDescriptorSetLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = 2;
    layout_info.pBindings = global_texture_bindings;

	if(vkCreateDescriptorSetLayout(logical_device, &layout_info, nullptr, &tex_descriptor_set_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor set layout.");
	}
}
void create_graphics_pipeline()
{
	ReadEntireFile vert_shader_code = read_entire_file(vert_file);
	ReadEntireFile frag_shader_code = read_entire_file(frag_file);

	VkShaderModuleCreateInfo vertShaderCreateInfo{};
	vertShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertShaderCreateInfo.codeSize = vert_shader_code.contents_size;
	vertShaderCreateInfo.pCode = (uint32_t*)(vert_shader_code.contents);

	VkShaderModuleCreateInfo fragShaderCreateInfo{};
	fragShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragShaderCreateInfo.codeSize = frag_shader_code.contents_size;
	fragShaderCreateInfo.pCode = (uint32_t*)(frag_shader_code.contents);

	if(vkCreateShaderModule(logical_device, &vertShaderCreateInfo, nullptr, &vertShaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module.");
	}
	if(vkCreateShaderModule(logical_device, &fragShaderCreateInfo, nullptr, &fragShaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module.");
	}

	VkPipelineShaderStageCreateInfo vert_stage{};
	vert_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_stage.module = vertShaderModule;
	vert_stage.pName = "main";

	VkPipelineShaderStageCreateInfo frag_stage{};
	frag_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_stage.module = fragShaderModule;
	frag_stage.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { frag_stage, vert_stage};

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.pViewports = &viewport;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.pScissors = &scissor;

	VkVertexInputBindingDescription binding_description = Vertex::getBindingDescription();
	VkVertexInputAttributeDescription* attribute_descriptions = Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = 3;
	vertexInputInfo.pVertexBindingDescriptions = &binding_description;
	vertexInputInfo.pVertexAttributeDescriptions = attribute_descriptions;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
	rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerCreateInfo.depthClampEnable = VK_FALSE;
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizerCreateInfo.lineWidth = 1.0f;
	rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizerCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
	rasterizerCreateInfo.depthBiasClamp = 0.0f; // Optional
	rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo{};
	multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
	multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisamplingCreateInfo.minSampleShading = 1.0f;
	multisamplingCreateInfo.pSampleMask = nullptr;
	multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo{};
	colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendingCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendingCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendingCreateInfo.attachmentCount = 1;
	colorBlendingCreateInfo.pAttachments = &colorBlendAttachment;
	colorBlendingCreateInfo.blendConstants[0] = 0.0f;
	colorBlendingCreateInfo.blendConstants[1] = 0.0f;
	colorBlendingCreateInfo.blendConstants[2] = 0.0f;
	colorBlendingCreateInfo.blendConstants[3] = 0.0f;

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	VkDescriptorSetLayout layouts[2] = {UBO_descriptor_set_layout, tex_descriptor_set_layout};

	VkPushConstantRange pushConstantRange = {};
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(uint32_t);
	pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 2;
	pipelineLayoutCreateInfo.pSetLayouts = layouts;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

	if(vkCreatePipelineLayout(logical_device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout.");
	}

	VkPipelineRenderingCreateInfoKHR pipeline_rendering_info{};
	pipeline_rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
	pipeline_rendering_info.colorAttachmentCount = 1,
	pipeline_rendering_info.pColorAttachmentFormats = &surface_format.format;
	pipeline_rendering_info.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = &pipeline_rendering_info;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
	pipelineInfo.pViewportState = &viewportStateCreateInfo;
	pipelineInfo.pRasterizationState = &rasterizerCreateInfo;
	pipelineInfo.pMultisampleState = &multisamplingCreateInfo;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlendingCreateInfo;
	pipelineInfo.pDynamicState = nullptr; // Optional
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if(vkCreateGraphicsPipelines(logical_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create graphics pipeline.");
	}

	vkDestroyShaderModule(logical_device, fragShaderModule, 0);
	vkDestroyShaderModule(logical_device, vertShaderModule, 0);
}
void create_commandpool()
{
	VkCommandPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolCreateInfo.queueFamilyIndex = graphics_family;

	if(vkCreateCommandPool(logical_device, &poolCreateInfo, nullptr, &command_pool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}
void create_depth_resources()
{
	depth_image.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
	depth_image.width = extent.width;
	depth_image.height = extent.height;
	create_image(&depth_image, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

	VkCommandBuffer command_buffer = begin_single_command_buffer();
	set_image_layout(command_buffer, depth_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	end_single_command_buffer(command_buffer);
}
void create_texture_image(Image* image, const char* path)
{
	stbi_uc* pixels = stbi_load(path, &image->width, &image->height, &image->channels,
	STBI_rgb_alpha);

	image->format = VK_FORMAT_R8G8B8A8_SRGB;
	VkDeviceSize image_size = (uint64_t)image->width * (uint64_t)image->height * 4;

	if(!pixels) 
	{
		throw std::runtime_error("Failed to load texture image.");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	create_buffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(logical_device, stagingBufferMemory, 0, image_size, 0, &data);
		memcpy(data, pixels, (size_t)(image_size));
	vkUnmapMemory(logical_device, stagingBufferMemory);

	stbi_image_free(pixels);

	create_image(image, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

	VkCommandBuffer command_buffer = begin_single_command_buffer();

	set_image_layout(command_buffer, image->image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copy_buffer_to_image(command_buffer, stagingBuffer, image);
	set_image_layout(command_buffer, image->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	end_single_command_buffer(command_buffer);

	vkDestroyBuffer(logical_device, stagingBuffer, nullptr);
	vkFreeMemory(logical_device, stagingBufferMemory, nullptr);
}
void create_texture_sampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = physical_device_prop.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if(vkCreateSampler(logical_device, &samplerInfo, nullptr, &image_sampler) != VK_SUCCESS) 
	{
		throw std::runtime_error("Failed to create texture sampler.");
	}
}
void load_model(const char* path)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	tinyobj::LoadObj(&attrib, &shapes, &materials, 0, 0, path);

	for(uint32_t i = 0; i < shapes.size(); i++) 
	{
		for(tinyobj::index_t &index: shapes[i].mesh.indices) 
		{
			Vertex vertex{};
			vertex.pos =
			{
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};
			vertex.tex_coords =
			{
				attrib.texcoords[2 * index.texcoord_index + 0],
				attrib.texcoords[2 * index.texcoord_index + 1]
			};
			vertex.color = { 1.0f, 1.0f, 1.0f };

			global_model_vertices.push_back(vertex);
			global_model_indices.push_back(global_model_indices.size());
		}
	}
	VkDeviceSize vertex_offset = global_model_vertices.size();
	VkDeviceSize index_offset = global_model_indices.size();
}
void create_vertex_buffer()
{
	VkDeviceSize buffer_size = sizeof(global_model_vertices[0]) * global_model_vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(logical_device, stagingBufferMemory, 0, buffer_size, 0, &data);
	memcpy(data, global_model_vertices.data(), buffer_size);
	vkUnmapMemory(logical_device, stagingBufferMemory);

	create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	copy_buffer(stagingBuffer, vertexBuffer, buffer_size);

	vkDestroyBuffer(logical_device, stagingBuffer, nullptr);
	vkFreeMemory(logical_device, stagingBufferMemory, nullptr);
}
void create_index_buffer()
{
	VkDeviceSize buffer_size = sizeof(global_model_indices[0]) * global_model_indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(logical_device, stagingBufferMemory, 0, buffer_size, 0, &data);
	memcpy(data, global_model_indices.data(), buffer_size);
	vkUnmapMemory(logical_device, stagingBufferMemory);

	create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copy_buffer(stagingBuffer, indexBuffer, buffer_size);

	vkDestroyBuffer(logical_device, stagingBuffer, nullptr);
	vkFreeMemory(logical_device, stagingBufferMemory, nullptr);
}
void create_uniform_buffer()
{
	VkDeviceSize buffer_size = sizeof(UBO);

	create_buffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	uniformBuffer, uniformBufferMemory);

	vkMapMemory(logical_device, uniformBufferMemory, 0, buffer_size, 0, &uniformBufferMapped);
}
void create_descriptor_pool(VkDescriptorType pool_type, VkDescriptorPool* pool_addr, 
							uint32_t max_sets, uint32_t descriptor_count)
{
	VkDescriptorPoolSize pool_size{};
	pool_size.type = pool_type;
	pool_size.descriptorCount = descriptor_count;

	VkDescriptorPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = 1;
	pool_info.pPoolSizes = &pool_size;
	pool_info.maxSets = max_sets;

	if(vkCreateDescriptorPool(logical_device, &pool_info, nullptr, pool_addr) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor pool.");
	}
}
void create_UBO_descriptor_set()
{
	VkDescriptorSetAllocateInfo setAllocInfo{};
	setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocInfo.descriptorPool = UBO_descriptor_pool;
	setAllocInfo.descriptorSetCount = 1;
	setAllocInfo.pSetLayouts = &UBO_descriptor_set_layout;

	if(vkAllocateDescriptorSets(logical_device, &setAllocInfo, &UBO_descriptor_set) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate descriptor set.");
	}

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = uniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UBO);

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = UBO_descriptor_set;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(logical_device, 1, &descriptorWrite, 0, nullptr);

}
void create_tex_descriptor_sets()
{
	VkDescriptorSetAllocateInfo setAllocInfo{};
	setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocInfo.descriptorPool = tex_descriptor_pool;
	setAllocInfo.descriptorSetCount = 1;
	setAllocInfo.pSetLayouts = &tex_descriptor_set_layout;

	Image descriptor_images[2];
	descriptor_images[0] = image_one;
	descriptor_images[1] = image_two;

	if(vkAllocateDescriptorSets(logical_device, &setAllocInfo, &tex_descriptor_set) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate descriptor set.");
	}

	VkDescriptorImageInfo image_infos[2];
	for(uint32_t i = 0; i < 2; i++)
	{
		image_infos[i].sampler = nullptr;
		image_infos[i].imageView = descriptor_images[i].view;
		image_infos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	VkDescriptorImageInfo sampler_info = {};
	sampler_info.sampler = image_sampler;

	VkWriteDescriptorSet descriptorWriteImage[2] = {};
	descriptorWriteImage[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWriteImage[0].dstSet = tex_descriptor_set;
	descriptorWriteImage[0].dstBinding = 0;
	descriptorWriteImage[0].dstArrayElement = 0;
	descriptorWriteImage[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	descriptorWriteImage[0].descriptorCount = 1;
	descriptorWriteImage[0].pImageInfo = &sampler_info;

	descriptorWriteImage[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWriteImage[1].dstSet = tex_descriptor_set;
	descriptorWriteImage[1].dstBinding = 1;
	descriptorWriteImage[1].dstArrayElement = 0;
	descriptorWriteImage[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	descriptorWriteImage[1].descriptorCount = 2;
	descriptorWriteImage[1].pImageInfo = image_infos;

	vkUpdateDescriptorSets(logical_device, 2, descriptorWriteImage, 0, nullptr);
}
void create_command_buffers()
{
	VkCommandBufferAllocateInfo allocCreateInfo{};
	allocCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocCreateInfo.commandPool = command_pool;
	allocCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocCreateInfo.commandBufferCount = 1;

	if(vkAllocateCommandBuffers(logical_device, &allocCreateInfo, &command_buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers.");
	}
}
void create_sync_objects()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if(vkCreateSemaphore(logical_device, &semaphoreCreateInfo, nullptr, &image_available_semaphores) != VK_SUCCESS ||
		vkCreateSemaphore(logical_device, &semaphoreCreateInfo, nullptr, &render_finished_semaphores) != VK_SUCCESS ||
		vkCreateFence(logical_device, &fenceCreateInfo, nullptr, &in_flight_fences) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create semaphores.");
		}

}
									
void init_vulkan(HWND win32_handle)
{
	create_instance();
	create_surface(win32_handle);
	create_logical_device();
	create_swapchain();

	create_UBO_descriptor_set_layout();
	create_tex_descriptor_set_layout();

	create_graphics_pipeline();
	create_commandpool();
	create_depth_resources();

	create_texture_image(&image_one, furnace_tex_path);
	create_texture_image(&image_two, wood_tex_path);
	create_texture_sampler();

	load_model(model_path);
	load_model(room_path);

	create_vertex_buffer();
	create_index_buffer();
	create_uniform_buffer();

	create_descriptor_pool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
						   &UBO_descriptor_pool, 1,2);

	create_descriptor_pool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						   &tex_descriptor_pool, 1,2);

	create_UBO_descriptor_set();
	create_tex_descriptor_sets();

	create_command_buffers();
	create_sync_objects();
}