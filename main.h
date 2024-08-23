#pragma once

// INCLUDES
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <DirectXMath.h>

#include <ktx.h>
#include <ktxvulkan.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdexcept>
#include <iostream>
#include <fstream>

#include <vector>
#include <set>
#include <optional>
#include <array>

#include <limits>
#include <algorithm> //clamp
#include <chrono>

#define Deg2Rad 0.0174532925f
constexpr const char* NULLPTR_MESSAGE{ "some parameters are nullptr!" };


// NAMESPACES
using namespace std;
using namespace DirectX;


//STRUCTS
struct Queue_Families {
	optional<uint32_t> graphicsFamily;
	optional<uint32_t> presentFamily;

	bool isComplete() {
		if (graphicsFamily.has_value() && presentFamily.has_value()) {
			return true;
		}

		return false;
	}
};

struct Vertex {
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
	XMFLOAT4 col;
};

struct Transform {
	XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
};

struct Buffer {
	VkDeviceSize bufferSize = 0;
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
};

struct Model {
	uint32_t vertexCount = 0;
	Buffer vertexBuffer;
	uint32_t indexCount = 0;
	Buffer indexBuffer;
};

struct Texture {
	char filename[256] = {'\0'};
	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory imageMemory = VK_NULL_HANDLE;
	VkImageView imageView = VK_NULL_HANDLE;
};

struct Object {
	bool render = true;
	bool update = false;
	Transform transform;
	XMMATRIX W = XMMatrixIdentity();
	XMMATRIX normalW = XMMatrixIdentity();

	uint32_t modelIndex = 0;
	uint32_t textureIndex = 0;
};

struct Light {
	alignas(16) XMFLOAT3 pos;
	alignas(16) XMFLOAT3 dir;
	alignas(16) XMFLOAT2 rot;
	alignas(16) XMFLOAT4 col;
	alignas(16) float intensity;
};

struct Image {
	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory imageMemory = VK_NULL_HANDLE; 
	VkImageView imageView = VK_NULL_HANDLE;
	VkFramebuffer framebuffer = VK_NULL_HANDLE;
};



// helper functions
bool createShaderModule(VkDevice device, const char* filename, VkShaderModule* pShaderModule);
bool loadByteCode(const char* filename, vector<char>* pByteCode);

XMMATRIX getViewMatrixPointLight(XMFLOAT3 lightPosition, XMFLOAT3 focusDirectionF3, XMFLOAT3 upDirectionF3);
XMMATRIX getViewMatrixLight(XMFLOAT3 lightPosition, XMFLOAT3 lightDirection);

bool loadMesh(Assimp::Importer* pImporter, const char* filename, vector<Vertex>* pVertices, vector<uint32_t>* pIndices);
bool createModel(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkPhysicalDevice physicalDevice, VkDevice device, const vector<Vertex>* pVertices, const vector<uint32_t>* pIndices, Model* pModel);

XMMATRIX TRS(Transform transform, XMMATRIX RS_M);
XMMATRIX RS(Transform transform);

bool endOneTimeCommandBuffer(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkDevice device, VkCommandBuffer* pOneTimeCommandBuffer);
bool beginOneTimeCommandBuffer(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkDevice device, VkCommandBuffer* pOneTimeCommandBuffer);
bool createStaticBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags propertyFlags, VkPhysicalDevice physicalDevice, VkDevice device, VkBuffer* pBuffer, VkDeviceMemory* pBufferMemory);
bool createImageView(VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspectMask, uint32_t mipLevels, uint32_t arrayLayers, VkDevice device, VkImageView* pImageView);
bool createImage(VkImageCreateFlags flags, VkFormat imageFormat, VkExtent2D imageExtent, uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits msaaSampleCount, VkImageUsageFlags imageUsage, VkPhysicalDevice physicalDevice, VkDevice device, VkImage* pImage, VkDeviceMemory* pImageMemory);
bool findMemoryType(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags propertyFlags, uint32_t* pMemoryTypeIndex);