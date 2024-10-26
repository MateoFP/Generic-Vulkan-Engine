#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

#include <vulkan/vulkan.h>
#include "mateo_math.h"

typedef struct Vertex
{
	v3 pos;
	//v3	normal;
	v2	uv;
	uint32_t tex_index;
	uint32_t model_index;

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
		attributeDescriptions = (VkVertexInputAttributeDescription*)malloc(4 * sizeof(VkVertexInputAttributeDescription));

		attributeDescriptions[0].binding  = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		//attributeDescriptions[1].binding  = 0;
		//attributeDescriptions[1].location = 1;
		//attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		//attributeDescriptions[1].offset = offsetof(Vertex, normal);

		attributeDescriptions[1].binding  = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, uv);

		attributeDescriptions[2].binding  = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32_UINT;
		attributeDescriptions[2].offset = offsetof(Vertex, tex_index);

		attributeDescriptions[3].binding  = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32_UINT;
		attributeDescriptions[3].offset = offsetof(Vertex, model_index);

		return attributeDescriptions;
	}
} Vertex;

typedef struct GUBO
{
	mat4 model;
	mat4 view;
	mat4 proj;
	mat4 projView;
} GUBO;

typedef struct ReadEntireFile
{
	char *contents;
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

typedef struct Buffer
{
	VkDeviceMemory	mem;
	VkBuffer		buffer;
} Buffer;

