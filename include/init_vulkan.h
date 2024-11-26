#pragma once
#include "vulkan_backend.h"
#include "stb_image.h"
#include "tiny_obj_loader.h"

#define MODEL_NUM	3
#define TEXTURE_NUM 3
#define WIDTH		1600
#define HEIGHT		900	

const char* room_path			= "C:/dev/myVulkan-1/resources/models/room.obj";
const char* model_path			= "C:/dev/myVulkan-1/resources/models/model.obj";
const char* room_tex_path		= "C:/dev/myVulkan-1/resources/textures/checkered.png";
const char* model_tex_path		= "C:/dev/myVulkan-1/resources/textures/cat.png";
const char* material_tex_path	= "C:/dev/myVulkan-1/resources/textures/material.png";
const char* frag_file			= "C:/dev/myVulkan-1/resources/shaders/frag_file.spv";
const char* vert_file			= "C:/dev/myVulkan-1/resources/shaders/vert_file.spv";

std::vector<Vertex> global_model_vertices;

GUBO		gubo{};
Image		depth_image{};
VkSampler	image_sampler;
Image		textures[3];

Buffer		model_uniform_buffer;
void		*model_uniform_buffer_mapped;
Buffer		global_uniform_buffer;
void		*global_uniform_buffer_mapped;
Buffer		global_vertex_buffer;
Buffer		global_index_buffer;

VkDescriptorSetLayout	layouts[2];
VkDescriptorSet			sets[2];
VkDescriptorPool		pools[2];

VkSemaphore			image_available_semaphores;
VkSemaphore			render_finished_semaphores;
VkFence				in_flight_fences;

VkCommandBuffer		gcommand_buffer;
VkCommandPool		gcommand_pool;

VkPipeline			graphics_pipeline;
VkPipelineLayout	pipeline_layout;

VkSwapchainKHR				swapchain;
VkImageView					*swapchain_image_views;
VkImage						*swapchain_images;
uint32_t					image_count;
VkExtent2D					extent;
VkSurfaceFormatKHR			surface_format;
VkPresentModeKHR			surface_present_mode;
VkSurfaceCapabilitiesKHR	surface_cap;
VkSurfaceKHR				surface;

VkDevice							logical_device;
VkQueue								graphics_queue;
uint32_t							graphics_family;
VkQueueFamilyProperties				*queue_family_prop;
uint32_t							queue_family_count;
VkPhysicalDeviceMemoryProperties	physical_device_mem_prop;
VkPhysicalDeviceFeatures			physical_device_feat;
VkPhysicalDeviceProperties			physical_device_prop;
VkPhysicalDevice					physical_device;

VkCommandBuffer begin_single_command_buffer(VkCommandPool* pool)
{
	VkCommandBufferAllocateInfo allocCreateInfo{};
	allocCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocCreateInfo.commandPool = gcommand_pool;
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
	vkFreeCommandBuffers(logical_device, gcommand_pool, 1, &command_buffer);
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

	Assert(vkCreateImage(logical_device, &imageInfo, nullptr, &image->image) == VK_SUCCESS);

	VkMemoryRequirements mem_req;
	vkGetImageMemoryRequirements(logical_device, image->image, &mem_req);

	uint32_t mem_property = 0;
	for(uint32_t i = 0; i < physical_device_mem_prop.memoryTypeCount; i++)
	{
		if((mem_req.memoryTypeBits & (1 << i)) &&
			(physical_device_mem_prop.memoryTypes[i].propertyFlags & prop) == prop)
		{
			mem_property = i;
			break;
		}
	}

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = mem_req.size;
	allocInfo.memoryTypeIndex = mem_property;

	Assert(vkAllocateMemory(logical_device, &allocInfo, nullptr, &image->memory) == VK_SUCCESS);
	Assert(vkBindImageMemory(logical_device, image->image, image->memory, 0) == VK_SUCCESS);

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

	Assert(vkCreateImageView(logical_device, &viewInfo, nullptr, &image->view) == VK_SUCCESS);
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

void copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer command_buffer = begin_single_command_buffer(&gcommand_pool);

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

	Assert(vkCreateBuffer(logical_device, &bufferCreateInfo, 0, &buffer) == VK_SUCCESS);

	VkMemoryRequirements mem_req;
	vkGetBufferMemoryRequirements(logical_device, buffer, &mem_req);

	uint32_t mem_property = 0;
	for(uint32_t i = 0; i < physical_device_mem_prop.memoryTypeCount; i++)
	{
		if((mem_req.memoryTypeBits & (1 << i)) &&
			(physical_device_mem_prop.memoryTypes[i].propertyFlags & prop) == prop)
		{
			mem_property = i;
			break;
		}
	}

	VkMemoryAllocateInfo memoryAllocInfo{};
	memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocInfo.allocationSize = mem_req.size;
	memoryAllocInfo.memoryTypeIndex = mem_property;

	Assert(vkAllocateMemory(logical_device, &memoryAllocInfo, 0, &bufferMemory) == VK_SUCCESS);
	Assert(vkBindBufferMemory(logical_device, buffer, bufferMemory, 0) == VK_SUCCESS); //maybe not
}   
void record_gcommand_buffer(VkCommandBuffer command_buffer, uint32_t image_index)
{
	VkCommandBufferBeginInfo beingInfo{};
	beingInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	Assert(vkBeginCommandBuffer(command_buffer, &beingInfo) == VK_SUCCESS);

	set_image_layout(command_buffer, swapchain_images[image_index], 
					 VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

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

	vkCmdBeginRendering(command_buffer, &render_info);

		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);
		VkBuffer vertex_buffers[] = {global_vertex_buffer.buffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
		vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout,
								0, 2, sets, 0, nullptr);
		//vkCmdDraw(command_buffer, global_model_vertices.size(), 1, 0, 0);
		vkCmdDraw(command_buffer, global_model_vertices.size() -4896, 1, 4896, 0);
		vkCmdDraw(command_buffer, 2448, 1, 0, 0);
		vkCmdDraw(command_buffer, 2448, 1, 2448, 0);

	vkCmdEndRendering(command_buffer);

	set_image_layout(command_buffer, swapchain_images[image_index], 
					 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	Assert(vkEndCommandBuffer(command_buffer) == VK_SUCCESS);
}

VkInstance create_instance(const char* instanceLayers)
{
	const char* instanceExtensions[] = {VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
										VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};

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
	instanceCreateInfo.enabledExtensionCount = ArrayCount(instanceExtensions);
	instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions;
	instanceCreateInfo.enabledLayerCount = 1;
	instanceCreateInfo.ppEnabledLayerNames = &instanceLayers;

	VkInstance my_instance;
	Assert(vkCreateInstance(&instanceCreateInfo, 0, &my_instance) == VK_SUCCESS);

	return my_instance;
}
void create_surface(VkInstance my_instance, HWND win32_handle)
{
	VkWin32SurfaceCreateInfoKHR surface_create_info = {};
	surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surface_create_info.hinstance = GetModuleHandleW(NULL);
	surface_create_info.hwnd = win32_handle;

	Assert(vkCreateWin32SurfaceKHR(my_instance, &surface_create_info, NULL, &surface) == VK_SUCCESS);
}
void create_logical_device(VkInstance my_instance, const char* instanceLayers)
{
	//pick physical device
	uint32_t deviceCount = 1;
	queue_family_count = 0;

	vkEnumeratePhysicalDevices(my_instance, &deviceCount, &physical_device);
	vkGetPhysicalDeviceProperties(physical_device, &physical_device_prop);
	vkGetPhysicalDeviceFeatures(physical_device, &physical_device_feat);
	vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_mem_prop);

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
	deviceInfo.ppEnabledLayerNames = &instanceLayers;
	deviceInfo.enabledExtensionCount = ArrayCount(deviceExtensions);
	deviceInfo.ppEnabledExtensionNames = deviceExtensions;

	Assert(vkCreateDevice(physical_device, &deviceInfo, 0, &logical_device) == VK_SUCCESS);
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

	surface_format = surface_formats[4];
	surface_present_mode = surface_present_modes[1];
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

	Assert(vkCreateSwapchainKHR(logical_device, &swapchainCreateInfo, nullptr, &swapchain) == VK_SUCCESS);

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

		Assert(vkCreateImageView(logical_device, &ImageViewCreateInfo, 0, &swapchain_image_views[i]) == VK_SUCCESS);
	}
}
void create_descriptor_set_layouts()
{
	//ubo descriptor set layout
	VkDescriptorSetLayoutBinding uboLayoutBinding[2];
	uboLayoutBinding[0].binding = 0;
	uboLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding[0].descriptorCount = 1;
	uboLayoutBinding[0].pImmutableSamplers = nullptr;
	uboLayoutBinding[0].stageFlags = VK_SHADER_STAGE_ALL;

	uboLayoutBinding[1].binding = 1;
	uboLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding[1].descriptorCount = 1;
	uboLayoutBinding[1].pImmutableSamplers = nullptr;
	uboLayoutBinding[1].stageFlags = VK_SHADER_STAGE_ALL;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 2;
	layoutInfo.pBindings = uboLayoutBinding;

	Assert(vkCreateDescriptorSetLayout(logical_device, &layoutInfo, nullptr, &layouts[0]) == VK_SUCCESS);

	//texture descriptor set layout
    VkDescriptorSetLayoutBinding global_texture_bindings[2] = {};
	global_texture_bindings[0].binding = 0;
	global_texture_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	global_texture_bindings[0].descriptorCount = 1;
	global_texture_bindings[0].stageFlags = VK_SHADER_STAGE_ALL;

	global_texture_bindings[1].binding = 1;
	global_texture_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	global_texture_bindings[1].descriptorCount = TEXTURE_NUM;
	global_texture_bindings[1].stageFlags = VK_SHADER_STAGE_ALL;

    VkDescriptorSetLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = 2;
    layout_info.pBindings = global_texture_bindings;

	Assert(vkCreateDescriptorSetLayout(logical_device, &layout_info, nullptr, &layouts[1]) == VK_SUCCESS);
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

	VkShaderModule	vert_module;
	VkShaderModule	frag_module;

	Assert(vkCreateShaderModule(logical_device, &vertShaderCreateInfo, nullptr, &vert_module) == VK_SUCCESS);
	Assert(vkCreateShaderModule(logical_device, &fragShaderCreateInfo, nullptr, &frag_module) == VK_SUCCESS);

	VkPipelineShaderStageCreateInfo vert_stage{};
	vert_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_stage.module = vert_module;
	vert_stage.pName = "main";

	VkPipelineShaderStageCreateInfo frag_stage{};
	frag_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_stage.module = frag_module;
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
	vertexInputInfo.vertexAttributeDescriptionCount = 4;
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
	rasterizerCreateInfo.lineWidth = 5.0f;
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

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 2;
	pipelineLayoutCreateInfo.pSetLayouts = layouts;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = 0;

	Assert(vkCreatePipelineLayout(logical_device, &pipelineLayoutCreateInfo, nullptr, &pipeline_layout) == VK_SUCCESS);

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
	pipelineInfo.layout = pipeline_layout;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	Assert(vkCreateGraphicsPipelines(logical_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphics_pipeline) == VK_SUCCESS);

	vkDestroyShaderModule(logical_device, frag_module, 0);
	vkDestroyShaderModule(logical_device, vert_module, 0);
}
void create_command_pool(VkCommandPool* pool)
{
	VkCommandPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolCreateInfo.queueFamilyIndex = graphics_family;

	Assert(vkCreateCommandPool(logical_device, &poolCreateInfo, nullptr, pool) == VK_SUCCESS);
}
void create_depth_image()
{
	depth_image.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
	depth_image.width = extent.width;
	depth_image.height = extent.height;
	create_image(&depth_image, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

	VkCommandBuffer command_buffer = begin_single_command_buffer(&gcommand_pool);
	set_image_layout(command_buffer, depth_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	end_single_command_buffer(command_buffer);
}
void load_texture(Image* image, const char* texturepath)
{
	stbi_uc* pixels = stbi_load(texturepath, &image->width, &image->height, &image->channels,
	STBI_rgb_alpha);

	image->format = VK_FORMAT_R8G8B8A8_SRGB;
	VkDeviceSize image_size = (uint64_t)image->width * (uint64_t)image->height * 4;

	Assert(pixels);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	create_buffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	Assert(vkMapMemory(logical_device, stagingBufferMemory, 0, image_size, 0, &data) == VK_SUCCESS);
		memcpy(data, pixels, (size_t)(image_size));
	vkUnmapMemory(logical_device, stagingBufferMemory);

	stbi_image_free(pixels);

	create_image(image, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

	VkCommandBuffer command_buffer = begin_single_command_buffer(&gcommand_pool);

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
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = physical_device_prop.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	Assert(vkCreateSampler(logical_device, &samplerInfo, nullptr, &image_sampler) == VK_SUCCESS);
}
void load_model(const char* modelpath, uint32_t tex_id, uint32_t model_id)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;

	Assert(tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelpath, nullptr));

	//for every shape
	for(uint32_t s = 0; s < shapes.size(); s++) 
	{
		//for every index
		for(uint32_t i = 0; i < shapes[s].mesh.indices.size(); i++) 
		{
			tinyobj::index_t idx = shapes[s].mesh.indices[i];

			Vertex vert{};

			vert.pos.xyz[0] = attrib.vertices[3 * idx.vertex_index + 0];
			vert.pos.xyz[1] = attrib.vertices[3 * idx.vertex_index + 1];
			vert.pos.xyz[2] = attrib.vertices[3 * idx.vertex_index + 2];

			if(idx.texcoord_index >= 0)
			{
				vert.uv.xy[0] = attrib.texcoords[2 * idx.texcoord_index + 0];
				vert.uv.xy[1] = 2.0f - (attrib.texcoords[2 * idx.texcoord_index + 1]);
			}

			//vert.normal.x = attrib.normals[3 * idx.normal_index + 0];
			//vert.normal.y = attrib.normals[3 * idx.normal_index + 1];
			//vert.normal.z = attrib.normals[3 * idx.normal_index + 2];

			vert.tex_index   = tex_id;
			vert.model_index = model_id;

			global_model_vertices.push_back(vert);
		}
	}
}
void create_gvertex_buffer()
{
	VkDeviceSize buffer_size = sizeof(global_model_vertices[0]) * global_model_vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	Assert(vkMapMemory(logical_device, stagingBufferMemory, 0, buffer_size, 0, &data) == VK_SUCCESS);
	memcpy(data, global_model_vertices.data(), buffer_size);
	vkUnmapMemory(logical_device, stagingBufferMemory);

	create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, global_vertex_buffer.buffer, global_vertex_buffer.mem);

	copy_buffer(stagingBuffer, global_vertex_buffer.buffer, buffer_size);

	vkDestroyBuffer(logical_device, stagingBuffer, nullptr);
	vkFreeMemory(logical_device, stagingBufferMemory, nullptr);
}
void create_guniform_buffer()
{
	VkDeviceSize buffer_size = sizeof(GUBO);

	create_buffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				  global_uniform_buffer.buffer, global_uniform_buffer.mem);

	Assert(vkMapMemory(logical_device, global_uniform_buffer.mem, 0, buffer_size, 0, &global_uniform_buffer_mapped) == VK_SUCCESS);
}
void create_muniform_buffer()
{
	VkDeviceSize buffer_size = sizeof(mat4) *MODEL_NUM;

	create_buffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				  model_uniform_buffer.buffer, model_uniform_buffer.mem);

	Assert(vkMapMemory(logical_device, model_uniform_buffer.mem, 0, buffer_size, 0, &model_uniform_buffer_mapped) == VK_SUCCESS);
}
void create_descriptor_set(VkDescriptorType pool_type, VkDescriptorPool pool, uint32_t desc_count, 
						   VkDescriptorSetLayout* layout, VkDescriptorSet* set)
{
	VkDescriptorPoolSize pool_size{};
	pool_size.type = pool_type;
	pool_size.descriptorCount = desc_count;

	VkDescriptorPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = 1;
	pool_info.pPoolSizes = &pool_size;
	pool_info.maxSets = 1;

	Assert(vkCreateDescriptorPool(logical_device, &pool_info, nullptr, &pool) == VK_SUCCESS);

	VkDescriptorSetAllocateInfo setAllocInfo{};
	setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocInfo.descriptorPool = pool;
	setAllocInfo.descriptorSetCount = 1;
	setAllocInfo.pSetLayouts = layout;

	Assert(vkAllocateDescriptorSets(logical_device, &setAllocInfo, set) == VK_SUCCESS);
}
void write_descriptor_sets()
{
	//gUBO and mUBO 
	VkDescriptorBufferInfo gubo_info{};
	gubo_info.buffer = global_uniform_buffer.buffer;
	gubo_info.offset = 0;
	gubo_info.range = sizeof(GUBO);

	VkDescriptorBufferInfo mubo_info{};
	mubo_info.buffer = model_uniform_buffer.buffer;
	mubo_info.offset = 0;
	mubo_info.range = sizeof(mat4) *MODEL_NUM;

	VkWriteDescriptorSet write_ubo[2] = {};
	write_ubo[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write_ubo[0].dstSet = sets[0];
	write_ubo[0].dstBinding = 0;
	write_ubo[0].dstArrayElement = 0;
	write_ubo[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	write_ubo[0].descriptorCount = 1;
	write_ubo[0].pBufferInfo = &gubo_info;

	write_ubo[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write_ubo[1].dstSet = sets[0];
	write_ubo[1].dstBinding = 1;
	write_ubo[1].dstArrayElement = 0;
	write_ubo[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	write_ubo[1].descriptorCount = 1;
	write_ubo[1].pBufferInfo = &mubo_info;

	vkUpdateDescriptorSets(logical_device, 2, write_ubo, 0, nullptr);

	//sampler and textures
	VkDescriptorImageInfo image_infos[TEXTURE_NUM];
	for (uint32_t i = 0; i < TEXTURE_NUM; i++)
	{
		image_infos[i].sampler = nullptr;
		image_infos[i].imageView = textures[i].view;
		image_infos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	VkDescriptorImageInfo sampler_info = {};
	sampler_info.sampler = image_sampler;

	VkWriteDescriptorSet write_tex[2] = {};
	write_tex[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write_tex[0].dstSet = sets[1];
	write_tex[0].dstBinding = 0;
	write_tex[0].dstArrayElement = 0;
	write_tex[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	write_tex[0].descriptorCount = 1;
	write_tex[0].pImageInfo = &sampler_info;

	write_tex[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write_tex[1].dstSet = sets[1];
	write_tex[1].dstBinding = 1;
	write_tex[1].dstArrayElement = 0;
	write_tex[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	write_tex[1].descriptorCount = TEXTURE_NUM;
	write_tex[1].pImageInfo = image_infos;

	vkUpdateDescriptorSets(logical_device, 2, write_tex, 0, nullptr);
}
void create_gcommand_buffer()
{
	VkCommandBufferAllocateInfo allocCreateInfo{};
	allocCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocCreateInfo.commandPool = gcommand_pool;
	allocCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocCreateInfo.commandBufferCount = 1;

	Assert(vkAllocateCommandBuffers(logical_device, &allocCreateInfo, &gcommand_buffer) == VK_SUCCESS);
}
void create_sync_objects()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	Assert(vkCreateSemaphore(logical_device, &semaphoreCreateInfo, nullptr, &image_available_semaphores) == VK_SUCCESS);
	Assert(vkCreateSemaphore(logical_device, &semaphoreCreateInfo, nullptr, &render_finished_semaphores) == VK_SUCCESS);
	Assert(vkCreateFence(logical_device, &fenceCreateInfo, nullptr, &in_flight_fences) == VK_SUCCESS);
}

void init_vulkan(HWND win32_handle)
{
	const char instance_layers[] = {"VK_LAYER_KHRONOS_validation"};
	VkInstance instance = create_instance(instance_layers);

	create_surface(instance, win32_handle);
	create_logical_device(instance, instance_layers);
	create_swapchain();
	create_descriptor_set_layouts();
	create_graphics_pipeline();
	create_command_pool(&gcommand_pool);
	create_depth_image();

	create_texture_sampler();
	load_texture(&textures[0], model_tex_path);
	load_texture(&textures[1], room_tex_path);
	load_texture(&textures[2], material_tex_path);

	load_model(model_path, 0, 0);
	load_model(model_path, 2, 1);
	load_model(room_path, 1, 2);

	create_gvertex_buffer();
	create_guniform_buffer();
	create_muniform_buffer();

	create_descriptor_set(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, pools[0], 2, 
						  &layouts[0], &sets[0]);
	create_descriptor_set(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, pools[1], 3,
						  &layouts[1], &sets[1]);
	write_descriptor_sets();

	create_gcommand_buffer();
	create_sync_objects();
}