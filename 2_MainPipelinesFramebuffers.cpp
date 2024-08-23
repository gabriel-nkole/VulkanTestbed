#include "2_MainPipelinesFramebuffers.h"


bool createFramebuffers(VkRenderPass renderPass, VkExtent2D swapchainExtent, VkDevice device, VkImageView colorImageView, VkImageView depthImageView, vector<VkImageView>* pSwapchainImageViews, vector<VkFramebuffer>* pSwapchainFramebuffers) {
	if (pSwapchainImageViews == nullptr || pSwapchainFramebuffers == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}
	
	pSwapchainFramebuffers->resize(pSwapchainImageViews->size());

	VkFramebufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.renderPass = renderPass;
	createInfo.layers = 1;
	createInfo.width = swapchainExtent.width;
	createInfo.height = swapchainExtent.height;
	createInfo.attachmentCount = 3;

	for (size_t i = 0; i < pSwapchainImageViews->size(); i++) {
		VkImageView attachments[3] = {colorImageView, depthImageView, (*pSwapchainImageViews)[i]};
		createInfo.pAttachments = attachments;
		
		if (vkCreateFramebuffer(device, &createInfo, nullptr, &(*pSwapchainFramebuffers)[i]) != VK_SUCCESS) {
			cerr << "failed to create swap chain framebuffers! : index - " << i << endl;
			return false;
		}
	}

	return true;
}


bool createRenderPass(VkFormat swapchainFormat, VkFormat depthImageFormat, VkSampleCountFlagBits msaaSampleCount, VkDevice device, VkRenderPass* pRenderPass) {
	if (pRenderPass == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}
	
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchainFormat;
	colorAttachment.samples = msaaSampleCount;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = depthImageFormat;
	depthAttachment.samples = msaaSampleCount;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorResolveAttachment{};
	colorResolveAttachment.format = swapchainFormat;
	colorResolveAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorResolveAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorResolveAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorResolveAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorResolveAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorResolveAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorResolveAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription attachments[] = {colorAttachment, depthAttachment, colorResolveAttachment};


	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorResolveAttachmentRef{};
	colorResolveAttachmentRef.attachment = 2;
	colorResolveAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	subpass.pResolveAttachments = &colorResolveAttachmentRef;
	
	
	VkSubpassDependency subpassDependency{};
	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


	VkRenderPassCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = 3;
	createInfo.pAttachments = attachments;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpass;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &subpassDependency;

	if (vkCreateRenderPass(device, &createInfo, nullptr, pRenderPass) != VK_SUCCESS) {
		cerr << "failed to create render pass!" << endl;
		return false;
	}

	return true;
}


bool createDepthResources(VkFormat* pDepthImageFormat, VkExtent2D swapchainExtent, VkSampleCountFlagBits msaaSampleCount, VkCommandPool graphicsCommandPool, VkQueue graphicsQueue, VkPhysicalDevice physicalDevice, VkDevice device, VkImage* pDepthImage, VkDeviceMemory* pDepthImageMemory, VkImageView* pDepthImageView) {
	if (pDepthImageFormat == nullptr || pDepthImage == nullptr || pDepthImageMemory == nullptr || pDepthImageView == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}
	
	*pDepthImageFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;

	if (!createImage(0, *pDepthImageFormat, swapchainExtent, 1, 1, msaaSampleCount, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, physicalDevice, device, pDepthImage, pDepthImageMemory)) {
		cerr << "failed to create image for depth resources!" << endl;
		return false;
	}
	if (!createImageView(*pDepthImage, VK_IMAGE_VIEW_TYPE_2D, *pDepthImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 1, 1, device, pDepthImageView)) {
		cerr << "failed to create image view for depth resources!" << endl;
		return false;
	}

	

	VkCommandBuffer oneTimeCommandBuffer;
	if (!beginOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
		cerr << "failed to begin one-time command buffer for depth resources layout transition!" << endl;
		return false;
	}


	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcAccessMask = 0;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	imageMemoryBarrier.image = *pDepthImage;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;


	vkCmdPipelineBarrier(
		oneTimeCommandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier
	);

	
	if (!endOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
		cerr << "failed to end one-time command buffer for depth resources layout transition!" << endl;
		return false;
	}

	return true;
}

bool createColorResources(VkFormat swapchainFormat, VkExtent2D swapchainExtent, VkSampleCountFlagBits msaaSampleCount, VkCommandPool graphicsCommandPool, VkQueue graphicsQueue, VkPhysicalDevice physicalDevice, VkDevice device, VkImage* pColorImage, VkDeviceMemory* pColorImageMemory, VkImageView* pColorImageView) {
	if (pColorImage == nullptr || pColorImageMemory == nullptr || pColorImageView == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}
	
	if (!createImage(0, swapchainFormat, swapchainExtent, 1, 1, msaaSampleCount, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, physicalDevice, device, pColorImage, pColorImageMemory)) {
		cerr << "failed to create image for color resources!" << endl;
		return false;
	}
	if (!createImageView(*pColorImage, VK_IMAGE_VIEW_TYPE_2D, swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, device, pColorImageView)) {
		cerr << "failed to create image view for color resources!" << endl;
		return false;
	}


	VkCommandBuffer oneTimeCommandBuffer;
	if (!beginOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
		cerr << "failed to begin one-time command buffer for color resources layout transition!" << endl;
		return false;
	}


	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcAccessMask = 0;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	imageMemoryBarrier.image = *pColorImage;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;


	vkCmdPipelineBarrier(
		oneTimeCommandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier
	);

	
	if (!endOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
		cerr << "failed to end one-time command buffer for color resources layout transition!" << endl;
		return false;
	}

	return true;
}


bool createSwapchainImageViews(VkFormat swapchainFormat, VkDevice device, vector<VkImage>* pSwapchainImages, vector<VkImageView>* pSwapchainImageViews) {
	if (pSwapchainImages == nullptr || pSwapchainImageViews == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}

	pSwapchainImageViews->resize(pSwapchainImages->size());

	for (size_t i = 0; i < pSwapchainImageViews->size(); i++) {
		bool result = createImageView((*pSwapchainImages)[i], VK_IMAGE_VIEW_TYPE_2D, swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, device, &(*pSwapchainImageViews)[i]);
		if (!result) {
			cerr << "failed to create swapchain image views!" << endl;
			return false;
		}
	}

	return true;
}

bool createSwapchain(VkFormat* pSwapchainFormat, VkExtent2D* pSwapchainExtent, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow* window,
					 Queue_Families queueFamilies, VkDevice device, VkSwapchainKHR* pSwapchain, vector<VkImage>* pSwapchainImages) {
	if (pSwapchainFormat == nullptr || pSwapchainExtent == nullptr || pSwapchain == nullptr || pSwapchainImages == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}

	VkSurfaceCapabilitiesKHR surfaceCapabilities{};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

	uint32_t minImageCount = surfaceCapabilities.minImageCount + 1;
	if (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount) {
		minImageCount = surfaceCapabilities.maxImageCount;
	}

	VkExtent2D extent{};
	if (surfaceCapabilities.currentExtent.width != numeric_limits<uint32_t>::max()) {
		extent = surfaceCapabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		extent.width = clamp(static_cast<uint32_t>(width), surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		extent.height = clamp(static_cast<uint32_t>(height), surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
	}


	*pSwapchainFormat = VK_FORMAT_B8G8R8A8_SRGB;
	*pSwapchainExtent = extent;


	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = minImageCount;
	createInfo.imageFormat = *pSwapchainFormat;
	createInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	createInfo.imageExtent = *pSwapchainExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	
	if(queueFamilies.graphicsFamily.value() != queueFamilies.presentFamily.value()){
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		uint32_t indices[] = {queueFamilies.graphicsFamily.value(), queueFamilies.presentFamily.value()};
		createInfo.pQueueFamilyIndices = indices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	//createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;


	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, pSwapchain) != VK_SUCCESS) {
		cerr << "failed to create swap chain!" << endl;
		return false;
	}

	uint32_t swapChainImageCount = 0;
	vkGetSwapchainImagesKHR(device, *pSwapchain, &swapChainImageCount, nullptr);
	pSwapchainImages->resize(swapChainImageCount);
	vkGetSwapchainImagesKHR(device, *pSwapchain, &swapChainImageCount, pSwapchainImages->data());


	return true;
}