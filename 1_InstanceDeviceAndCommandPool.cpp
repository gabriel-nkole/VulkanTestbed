#include "1_InstanceDeviceAndCommandPool.h"


bool createGraphicsCommandPool(Queue_Families queueFamilies, VkDevice device, VkCommandPool* pGraphicsCommandPool) {
	if (pGraphicsCommandPool == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}

	VkCommandPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = queueFamilies.graphicsFamily.value();
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(device, &createInfo, nullptr, pGraphicsCommandPool) != VK_SUCCESS) {
		cerr << "failed to create graphics command pool!" << endl;
		return false;
	}

	return true;
}

bool createLogicalDevice(vector<const char*>* pRequiredDeviceExtensions, Queue_Families queueFamilies,
						 bool enableValidationLayers, vector<const char*>* pValidationLayers,
						 VkPhysicalDevice physicalDevice, VkDevice* pDevice, VkQueue* pGraphicsQueue, VkQueue* pPresentQueue) {
	if (pValidationLayers == nullptr || pDevice == nullptr || pGraphicsQueue == nullptr || pPresentQueue == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(pRequiredDeviceExtensions->size());
	createInfo.ppEnabledExtensionNames = pRequiredDeviceExtensions->data();
	

	// enabling device features
	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.sampleRateShading = VK_TRUE;
	deviceFeatures.fillModeNonSolid = VK_TRUE;
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	//createInfo.pEnabledFeatures = &deviceFeatures;


	VkPhysicalDeviceMultiviewFeaturesKHR physicalDeviceMultiviewFeatures{};
	physicalDeviceMultiviewFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;
	physicalDeviceMultiviewFeatures.multiview = VK_TRUE;

	VkPhysicalDeviceFeatures2 deviceFeatures2{};
	deviceFeatures2.features = deviceFeatures;
	deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceFeatures2.pNext = &physicalDeviceMultiviewFeatures;

	createInfo.pNext = &deviceFeatures2;



	// setting queue families
	vector<VkDeviceQueueCreateInfo> queueInfos{};
	set<uint32_t> queueFamilyIndices = { queueFamilies.graphicsFamily.value(), queueFamilies.presentFamily.value() };
	
	for (uint32_t queueFamilyIndex : queueFamilyIndices) {
		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = queueFamilyIndex;
		queueInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueInfo.pQueuePriorities = &queuePriority;
	
		queueInfos.push_back(queueInfo);
	}
	
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
	createInfo.pQueueCreateInfos = queueInfos.data();
	


	// validation layers
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(pValidationLayers->size());
		createInfo.ppEnabledLayerNames = pValidationLayers->data();
	}


	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, pDevice) != VK_SUCCESS) {
		cerr << "failed to create logical device!" << endl;
		return false;
	}
	
	vkGetDeviceQueue(*pDevice, queueFamilies.graphicsFamily.value(), 0, pGraphicsQueue);
	vkGetDeviceQueue(*pDevice, queueFamilies.presentFamily.value(), 0, pPresentQueue);

	return true;
}


Queue_Families findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
	uint32_t queueFamilyPropertyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertyCount, nullptr);
	vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertyCount, queueFamilyProperties.data());


	Queue_Families queueFamilies{};
	uint32_t i = 0;
	for (auto& queueFamilyProperty : queueFamilyProperties) {
		if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queueFamilies.graphicsFamily = i;
		}

		VkBool32 supported = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supported);
		if (supported) {
			queueFamilies.presentFamily = i;
		}

		if (queueFamilies.isComplete()) {
			return queueFamilies;
		}

		i++;
	}

	return queueFamilies;
}

bool getDeviceExtensionsSupport(VkPhysicalDevice device, vector<const char*>* pRequiredDeviceExtensions) {
	uint32_t propertyCount = 0;
	vkEnumerateDeviceExtensionProperties(device, NULL, &propertyCount, NULL);
	vector<VkExtensionProperties> extensionsProperties(propertyCount);
	vkEnumerateDeviceExtensionProperties(device, NULL, &propertyCount, extensionsProperties.data());

	set<const char*> requiredExtensionsSet(pRequiredDeviceExtensions->begin(), pRequiredDeviceExtensions->end());

	for (auto& providedExtensionProperty : extensionsProperties) {
		requiredExtensionsSet.erase(providedExtensionProperty.extensionName);
	}

	return !requiredExtensionsSet.empty();
}

bool isPhysicalDeviceSuitable(VkPhysicalDevice device, vector<const char*>* pRequiredDeviceExtensions, VkSurfaceKHR surface) {
	//VkPhysicalDeviceProperties properties{};
	//vkGetPhysicalDeviceProperties(device, &properties);

	bool deviceExtensionsSupport = getDeviceExtensionsSupport(device, pRequiredDeviceExtensions);
	Queue_Families queueFamilies = findQueueFamilies(device, surface);

	return deviceExtensionsSupport && queueFamilies.isComplete();
}

bool pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice* pPhysicalDevice, Queue_Families* pQueueFamilies, vector<const char*>* pRequiredDeviceExtensions) {
	if (pPhysicalDevice == nullptr || pQueueFamilies == nullptr || pRequiredDeviceExtensions == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}


	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
	vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

	for (auto& device : physicalDevices) {
		if (isPhysicalDeviceSuitable(device, pRequiredDeviceExtensions, surface)) {
			*pPhysicalDevice = device;
			*pQueueFamilies = findQueueFamilies(*pPhysicalDevice, surface);
		}
	}
	
	if (*pPhysicalDevice == VK_NULL_HANDLE) {
		cerr << "failed to find a suitable physical device!" << endl;
		return false;
	}

	//*pPhysicalDevice = physicalDevices[1];
	//*pQueueFamilies = findQueueFamilies(*pPhysicalDevice, surface);

	//cout << "Number of Physical Devices: " << physicalDeviceCount << endl;
	//cout << physicalDevices[0] << endl;

	return true;
}


bool createSurface(VkInstance instance, GLFWwindow* window, VkSurfaceKHR* pSurface) {
	if (window == nullptr || pSurface == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}

	if (glfwCreateWindowSurface(instance, window, nullptr, pSurface) != VK_SUCCESS) {
		cerr << "failed to create window surface!" << endl;
		return false;
	}

	return true;
}


void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr) {
		func(instance, messenger, pAllocator);
	}
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, VkAllocationCallbacks* pAllocators, VkDebugUtilsMessengerEXT* pMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (func == nullptr) {
		return VK_ERROR_UNKNOWN;
	}
	else {
		return func(instance, pCreateInfo, pAllocators, pMessenger);
	}
}

bool setupDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT* pMessenger) {
	if (pMessenger == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}
	
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateMessengerCreateInfo(&createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, pMessenger) != VK_SUCCESS) {
		cerr << "failed to create debug messenger!" << endl;
		return false;
	}

	return true;
}


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
												    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
												    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
												    void*                                       pUserData) {
	cerr << "Validation Layer: " << pCallbackData->pMessage << endl;			//errors from other functions may print between validation layer outputs

	return VK_FALSE;
}

void populateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pMessengerCreateInfo){
	pMessengerCreateInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	pMessengerCreateInfo->messageSeverity = /*VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |  VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |*/
									   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	pMessengerCreateInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	pMessengerCreateInfo->pfnUserCallback = debugCallBack;
}



bool getValidationLayerSupport(vector<const char*>* pRequiredValidationLayers) {
	uint32_t count = 0;
	vkEnumerateInstanceLayerProperties(&count, nullptr);
	vector<VkLayerProperties> providedLayersProperties(count);
	vkEnumerateInstanceLayerProperties(&count, providedLayersProperties.data());


	cout << "Required Validation Layers: " << pRequiredValidationLayers->size() << endl;
	for (size_t i = 0; i < pRequiredValidationLayers->size(); i++) {
		cout << "\t" << (*pRequiredValidationLayers)[i] << endl;
	}
	cout << endl;

	cout << "Available Layers: " << count << endl;
	for (size_t i = 0; i < count; i++) {
		cout << "\t" << providedLayersProperties[i].layerName << endl;
	}


	set<string> requiredLayersSet(pRequiredValidationLayers->begin(), pRequiredValidationLayers->end());
	for (auto& providedLayerProperty : providedLayersProperties) {
		requiredLayersSet.erase(providedLayerProperty.layerName);
	}
	cout << "Validation Layer Support: " << boolalpha << requiredLayersSet.empty() << endl << endl;


	return requiredLayersSet.empty();
}

bool getInstanceExtensionsSupport(vector<const char*>* pRequiredInstanceExtensions) {
	uint32_t count = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &count, nullptr);
	vector<VkExtensionProperties> availableExtensionsProperties(count);
	vkEnumerateInstanceExtensionProperties(NULL, &count, availableExtensionsProperties.data());


	cout << "Required Instance Extensions: " << pRequiredInstanceExtensions->size() << endl;
	for (size_t i = 0; i < pRequiredInstanceExtensions->size(); i++) {
		cout << "\t" << (*pRequiredInstanceExtensions)[i] << endl;
	}
	cout << endl;

	cout << "Available Instance Extensions: " << count << endl;
	for (size_t i = 0; i < count; i++) {
		cout << "\t" << availableExtensionsProperties[i].extensionName << endl;
	}


	set<string> requiredExtensionsSet(pRequiredInstanceExtensions->begin(), pRequiredInstanceExtensions->end());
	for (auto& providedExtensionProperty : availableExtensionsProperties) {
		requiredExtensionsSet.erase(providedExtensionProperty.extensionName);
	}
	cout << "Instance Extension Support: " << boolalpha << requiredExtensionsSet.empty() << endl << endl;


	return requiredExtensionsSet.empty();
}

vector<const char*> getRequiredInstanceExtensions(bool enableValidationLayers) {
	uint32_t count = 0;
	const char **requiredExtensionsArr = glfwGetRequiredInstanceExtensions(&count);
	
	vector<const char*> requiredExtensions(requiredExtensionsArr, requiredExtensionsArr + count);


	if (enableValidationLayers) {
		requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return requiredExtensions;
}

bool createInstance(bool enableValidationLayers, vector<const char*>* pRequiredValidationLayers, vector<const char*>* pRequiredDeviceExtensions, VkInstance* pInstance) {
	if (pRequiredValidationLayers == nullptr || pInstance == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}

	vector<const char*> requiredExtensions = getRequiredInstanceExtensions(enableValidationLayers);
	bool extensionSupport = getInstanceExtensionsSupport(&requiredExtensions);
	if (!extensionSupport) {
		cerr << "required extensions not supported!" << endl;
		return false;
	}

	if (enableValidationLayers) {
		bool validationLayerSupport = getValidationLayerSupport(pRequiredValidationLayers);
		if (!validationLayerSupport) {
			cerr << "required validation layers not supported!" << endl;
			return false;
		}
	}

	cout << "Required Device Extensions: " << pRequiredDeviceExtensions->size() << endl;
	for (size_t i = 0; i < pRequiredDeviceExtensions->size(); i++) {
		cout << "\t" << (*pRequiredDeviceExtensions)[i] << endl;
	}
	cout << endl;


	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.apiVersion = VK_API_VERSION_1_1;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pApplicationName = "My Vulkan";

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	createInfo.pApplicationInfo = &appInfo;

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(pRequiredValidationLayers->size());
		createInfo.ppEnabledLayerNames = pRequiredValidationLayers->data();

		VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
		populateMessengerCreateInfo(&messengerCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&messengerCreateInfo;
	}


	if (vkCreateInstance(&createInfo, nullptr, pInstance) != VK_SUCCESS) {
		cerr << "failed to create instance!" << endl;
		return false;
	}

	return true;
}