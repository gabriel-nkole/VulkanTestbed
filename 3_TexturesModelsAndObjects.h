#pragma once
#include "main.h"


// textures, models and objects
bool createModelsAndObjects(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkPhysicalDevice physicalDevice, VkDevice device, vector<Texture>* pAlbedomaps, vector<Model>* pModels, vector<Object>* pObjects);

bool createTextures(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkPhysicalDevice physicalDevice, VkDevice device, Texture* pSkyboxTexture, vector<Texture>* pAlbedomaps);
bool createTexture2D(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkPhysicalDevice physicalDevice, VkDevice device, Texture* p2DTexture);
bool createTextureSkybox(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkPhysicalDevice physicalDevice, VkDevice device, Texture* pSkyboxTexture);