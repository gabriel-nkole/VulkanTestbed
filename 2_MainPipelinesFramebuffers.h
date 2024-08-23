#pragma once
#include "main.h"


// main pipelines framebuffers
bool createFramebuffers(VkRenderPass renderPass, VkExtent2D swapchainExtent, VkDevice device, VkImageView colorImageView, VkImageView depthImageView, vector<VkImageView>* pSwapchainImageViews, vector<VkFramebuffer>* pSwapchainFramebuffers);
bool createRenderPass(VkFormat swapchainFormat, VkFormat depthImageFormat, VkSampleCountFlagBits msaaSampleCount, VkDevice device, VkRenderPass* pRenderPass);
bool createDepthResources(VkFormat* pDepthImageFormat, VkExtent2D swapchainExtent, VkSampleCountFlagBits msaaSampleCount, VkCommandPool graphicsCommandPool, VkQueue graphicsQueue, VkPhysicalDevice physicalDevice, VkDevice device, VkImage* pDepthImage, VkDeviceMemory* pDepthImageMemory, VkImageView* pDepthImageView);
bool createColorResources(VkFormat swapchainFormat, VkExtent2D swapchainExtent, VkSampleCountFlagBits msaaSampleCount, VkCommandPool graphicsCommandPool, VkQueue graphicsQueue, VkPhysicalDevice physicalDevice, VkDevice device, VkImage* pColorImage, VkDeviceMemory* pColorImageMemory, VkImageView* pColorImageView);

bool createSwapchainImageViews(VkFormat swapchainFormat, VkDevice device, vector<VkImage>* pSwapchainImages, vector<VkImageView>* pSwapchainImageViews);
bool createSwapchain(VkFormat* pSwapchainFormat, VkExtent2D* pSwapchainExtent, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow* window,
					 Queue_Families queueFamilies, VkDevice device, VkSwapchainKHR* pSwapchain, vector<VkImage>* pSwapchainImages);
