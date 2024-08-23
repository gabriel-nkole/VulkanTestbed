#pragma once
#include "main.h"


// instance, device, command pool
bool createGraphicsCommandPool(Queue_Families queueFamilies, VkDevice device, VkCommandPool* pGraphicsCommandPool);
bool createLogicalDevice(vector<const char*>* pRequiredDeviceExtensions, Queue_Families queueFamilies,
						 bool enableValidationLayers, vector<const char*>* pValidationLayers,
						 VkPhysicalDevice physicalDevice, VkDevice* pDevice, VkQueue* pGraphicsQueue, VkQueue* pPresentQueue);

Queue_Families findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
bool getDeviceExtensionsSupport(VkPhysicalDevice device, vector<const char*>* pRequiredDeviceExtensions);
bool isPhysicalDeviceSuitable(VkPhysicalDevice device, vector<const char*>* pRequiredDeviceExtensions, VkSurfaceKHR surface);
bool pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice* pPhysicalDevice, Queue_Families* pQueueFamilies, vector<const char*>* pRequiredDeviceExtensions);

bool createSurface(VkInstance instance, GLFWwindow* window, VkSurfaceKHR* pSurface);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator);
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, VkAllocationCallbacks* pAllocators, VkDebugUtilsMessengerEXT* pMessenger);
bool setupDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT* pMessenger);

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
												    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
												    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
												    void*                                       pUserData);
void populateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pMessengerCreateInfo);
bool getValidationLayerSupport(vector<const char*>* pRequiredValidationLayers);
bool getInstanceExtensionsSupport(vector<const char*>* pRequiredInstanceExtensions);
vector<const char*> getRequiredInstanceExtensions(bool enableValidationLayers);
bool createInstance(bool enableValidationLayers, vector<const char*>* pRequiredValidationLayers, vector<const char*>* pRequiredDeviceExtensions, VkInstance* pInstance);