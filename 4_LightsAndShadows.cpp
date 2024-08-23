#include "4_LightsAndShadows.h"


bool createShadowmapGraphicsPipelines(VkExtent2D shadowmapExtent, VkRenderPass dirSpotShadowmapRenderPass, VkRenderPass pointShadowmapRenderPass, VkDevice device,
									  VkPipelineLayout* pDirSpotShadowmapGraphicsPipelineLayout, VkPipeline* pDirSpotShadowmapGraphicsPipeline,
									  VkDescriptorSetLayout* pPointShadowmapDescriptorSetLayout, VkPipelineLayout* pPointShadowmapGraphicsPipelineLayout, VkPipeline* pPointShadowmapGraphicsPipeline) {
	if (pDirSpotShadowmapGraphicsPipelineLayout == nullptr || pDirSpotShadowmapGraphicsPipeline == nullptr ||
		pPointShadowmapGraphicsPipelineLayout == nullptr || pPointShadowmapGraphicsPipeline == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}
	
	
	VkGraphicsPipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	
	VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo{};
	vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageCreateInfo.pName = "main";
	vertexShaderStageCreateInfo.pSpecializationInfo = nullptr;
	
	VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo{};
	fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageCreateInfo.pName = "main";
	fragmentShaderStageCreateInfo.pSpecializationInfo = nullptr;



	// shadow directional & spot
	VkShaderModule shadow_DirSpot_vertexShaderModule = VK_NULL_HANDLE;
	if (!createShaderModule(device, "shaders/shadow_DirSpot.vert.spv", &shadow_DirSpot_vertexShaderModule)) {
		cerr << "failed to create shader module for shadow DirSpot vertex shader!" << endl;
		return false;
	}
	VkShaderModule shadow_DirSpot_fragmentShaderModule = VK_NULL_HANDLE;
	if (!createShaderModule(device, "shaders/shadow_DirSpot.frag.spv", &shadow_DirSpot_fragmentShaderModule)) {
		vkDestroyShaderModule(device, shadow_DirSpot_vertexShaderModule, nullptr);

		cerr << "failed to create shader module for shadow DirSpot fragment shader!" << endl;
		return false;
	}

	vertexShaderStageCreateInfo.module = shadow_DirSpot_vertexShaderModule;
	fragmentShaderStageCreateInfo.module = shadow_DirSpot_fragmentShaderModule;
	VkPipelineShaderStageCreateInfo shadowmap_DirSpot_shaderStages[] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };


	// shadow point
	VkShaderModule shadow_Point_vertexShaderModule = VK_NULL_HANDLE;
	if (!createShaderModule(device, "shaders/shadow_Point.vert.spv", &shadow_Point_vertexShaderModule)) {
		vkDestroyShaderModule(device, shadow_DirSpot_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, shadow_DirSpot_vertexShaderModule, nullptr);

		cerr << "failed to create shader module for shadow Point vertex shader!" << endl;
		return false;
	}
	VkShaderModule shadow_Point_fragmentShaderModule = VK_NULL_HANDLE;
	if (!createShaderModule(device, "shaders/shadow_Point.frag.spv", &shadow_Point_fragmentShaderModule)) {
		vkDestroyShaderModule(device, shadow_Point_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, shadow_DirSpot_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, shadow_DirSpot_vertexShaderModule, nullptr);

		cerr << "failed to create shader module for shadow Point fragment shader!" << endl;
		return false;
	}

	vertexShaderStageCreateInfo.module = shadow_Point_vertexShaderModule;
	fragmentShaderStageCreateInfo.module = shadow_Point_fragmentShaderModule;
	VkPipelineShaderStageCreateInfo shadowmap_Point_shaderStages[] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };



	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescription.stride = sizeof(Vertex);

	array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, Vertex::pos);


	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
	inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	

	VkViewport viewport;
	viewport.width = static_cast<float>(shadowmapExtent.width);
	viewport.height = static_cast<float>(shadowmapExtent.height);
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor;
	scissor.extent = shadowmapExtent;
	scissor.offset = { 0, 0 };

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.pViewports = &viewport;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.pScissors = &scissor;

	
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
	rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	//rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
	//rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
	rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
	rasterizationStateCreateInfo.lineWidth = 1.0f;
	

	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
	multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleStateCreateInfo.minSampleShading = 1.0f;
	multisampleStateCreateInfo.pSampleMask = nullptr;
	multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
	

	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
	depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilStateCreateInfo.minDepthBounds = 0.0f;
	depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
	depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;


	VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
	colorBlendAttachmentState.blendEnable = VK_FALSE;
	colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
	colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendStateCreateInfo.attachmentCount = 1;
	colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;

	colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	colorBlendStateCreateInfo.blendConstants[3] = 0.0f;



	array<VkPushConstantRange, 2> ranges{};
	ranges[0].offset = 0;
	ranges[0].size = sizeof(XMMATRIX);
	ranges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	ranges[1].offset = 0;
	ranges[1].size = sizeof(XMMATRIX) + sizeof(uint32_t);
	ranges[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	VkPipelineLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.setLayoutCount = 0;
	layoutCreateInfo.pSetLayouts = nullptr;
	layoutCreateInfo.pushConstantRangeCount = 1;
	layoutCreateInfo.pPushConstantRanges = &ranges[0];
	if (vkCreatePipelineLayout(device, &layoutCreateInfo, nullptr, pDirSpotShadowmapGraphicsPipelineLayout) != VK_SUCCESS) {
		vkDestroyShaderModule(device, shadow_Point_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, shadow_Point_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, shadow_DirSpot_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, shadow_DirSpot_vertexShaderModule, nullptr);
		
		cerr << "failed to create DirSpot shadow map graphics pipeline layout!" << endl;
		return false;
	}


	layoutCreateInfo.setLayoutCount = 1;
	layoutCreateInfo.pSetLayouts = pPointShadowmapDescriptorSetLayout;
	layoutCreateInfo.pPushConstantRanges = &ranges[1];
	if (vkCreatePipelineLayout(device, &layoutCreateInfo, nullptr, pPointShadowmapGraphicsPipelineLayout) != VK_SUCCESS) {
		vkDestroyShaderModule(device, shadow_Point_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, shadow_Point_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, shadow_DirSpot_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, shadow_DirSpot_vertexShaderModule, nullptr);
		
		cerr << "failed to create Point shadow map graphics pipeline layout!" << endl;
		return false;
	}
	


	createInfo.stageCount = 2;
	createInfo.pVertexInputState = &vertexInputStateCreateInfo;
	createInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	createInfo.pTessellationState = nullptr;
	createInfo.pViewportState = &viewportStateCreateInfo;
	createInfo.pRasterizationState = &rasterizationStateCreateInfo;
	createInfo.pMultisampleState = &multisampleStateCreateInfo;
	createInfo.pDepthStencilState = &depthStencilStateCreateInfo;
	createInfo.pColorBlendState = &colorBlendStateCreateInfo;
	createInfo.pDynamicState = nullptr;
	createInfo.subpass = 0;
	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = -1;


	VkGraphicsPipelineCreateInfo dirSpotShadowmap_createInfo = createInfo;
	dirSpotShadowmap_createInfo.pStages = shadowmap_DirSpot_shaderStages;
	dirSpotShadowmap_createInfo.layout = *pDirSpotShadowmapGraphicsPipelineLayout;
	dirSpotShadowmap_createInfo.renderPass = dirSpotShadowmapRenderPass;

	VkGraphicsPipelineCreateInfo pointShadowmap_createInfo = createInfo;
	pointShadowmap_createInfo.pStages = shadowmap_Point_shaderStages;
	pointShadowmap_createInfo.layout = *pPointShadowmapGraphicsPipelineLayout;
	pointShadowmap_createInfo.renderPass = pointShadowmapRenderPass;




	array<VkGraphicsPipelineCreateInfo, 2> arr_createInfo = { dirSpotShadowmap_createInfo, pointShadowmap_createInfo };
	array<VkPipeline, 2> graphicsPipelines{};
	
	if (vkCreateGraphicsPipelines(device, NULL, 2, arr_createInfo.data(), nullptr, graphicsPipelines.data()) != VK_SUCCESS) {
		vkDestroyShaderModule(device, shadow_Point_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, shadow_Point_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, shadow_DirSpot_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, shadow_DirSpot_vertexShaderModule, nullptr);

		cerr << "failed to create shadow map graphics pipelines!" << endl;
		return false;
	}
	memcpy(pDirSpotShadowmapGraphicsPipeline, &graphicsPipelines[0], sizeof(VkPipeline));
	memcpy(pPointShadowmapGraphicsPipeline, &graphicsPipelines[1], sizeof(VkPipeline));


	vkDestroyShaderModule(device, shadow_Point_fragmentShaderModule, nullptr);
	vkDestroyShaderModule(device, shadow_Point_vertexShaderModule, nullptr);
	vkDestroyShaderModule(device, shadow_DirSpot_fragmentShaderModule, nullptr);
	vkDestroyShaderModule(device, shadow_DirSpot_vertexShaderModule, nullptr);

	return true;
}

bool createPointShadowmapDescriptorResources(Buffer numLightsBuffer, Buffer pointLightVPMatricesBuffer, Buffer pointLightsBuffer,
											 VkDevice device, VkDescriptorSetLayout* pPointShadowmapDescriptorSetLayout, VkDescriptorPool* pPointShadowmapDescriptorPool, VkDescriptorSet* pPointShadowmapDescriptorSet) {
	if (pPointShadowmapDescriptorSetLayout == nullptr || pPointShadowmapDescriptorPool == nullptr || pPointShadowmapDescriptorSet == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}


	// descriptor set layout
	array<VkDescriptorSetLayoutBinding, 3> layoutBindings{};
	layoutBindings[0].binding = 0;
	layoutBindings[0].descriptorCount = 1;
	layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	layoutBindings[1].binding = 1;
	layoutBindings[1].descriptorCount = 1;
	layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	layoutBindings[2].binding = 2;
	layoutBindings[2].descriptorCount = 1;
	layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
	descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();
	if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, pPointShadowmapDescriptorSetLayout) != VK_SUCCESS) {
		cerr << "failed to create point shadowmap descriptor set layout!" << endl;
		return false;
	}
	
	
	
	// descriptor pool
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 3;
	
	
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolCreateInfo.maxSets = 1;
	descriptorPoolCreateInfo.poolSizeCount = 1;
	descriptorPoolCreateInfo.pPoolSizes = &poolSize;
	
	if (vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, pPointShadowmapDescriptorPool) != VK_SUCCESS) {
		cerr << "failed to create point shadowmap descriptor pool!" << endl;
		return false;
	}
	
	
	
	// allocate descriptor sets
	VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
	descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocInfo.pSetLayouts = pPointShadowmapDescriptorSetLayout;
	descriptorSetAllocInfo.descriptorPool = *pPointShadowmapDescriptorPool;
	descriptorSetAllocInfo.descriptorSetCount = 1;
	
	if (vkAllocateDescriptorSets(device, &descriptorSetAllocInfo, pPointShadowmapDescriptorSet) != VK_SUCCESS) {
		cerr << "failed to allocate point shadowmap descriptor sets!" << endl;
		return false;
	}



	// write to descriptor sets
	vector<VkWriteDescriptorSet> descriptorWrites(3);


	VkDescriptorBufferInfo numLightsDescriptorBufferInfo{};
	numLightsDescriptorBufferInfo.buffer = numLightsBuffer.buffer;
	numLightsDescriptorBufferInfo.offset = 0;
	numLightsDescriptorBufferInfo.range = numLightsBuffer.bufferSize;

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = *pPointShadowmapDescriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &numLightsDescriptorBufferInfo;


	VkDescriptorBufferInfo pointLightVPMatricesDescriptorBufferInfo{};
	pointLightVPMatricesDescriptorBufferInfo.buffer = pointLightVPMatricesBuffer.buffer;
	pointLightVPMatricesDescriptorBufferInfo.offset = 0;
	pointLightVPMatricesDescriptorBufferInfo.range = pointLightVPMatricesBuffer.bufferSize;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = *pPointShadowmapDescriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pBufferInfo = &pointLightVPMatricesDescriptorBufferInfo;


	VkDescriptorBufferInfo pointLightsDescriptorBufferInfo{};
	pointLightsDescriptorBufferInfo.buffer = pointLightsBuffer.buffer;
	pointLightsDescriptorBufferInfo.offset = 0;
	pointLightsDescriptorBufferInfo.range = pointLightsBuffer.bufferSize;

	descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[2].dstSet = *pPointShadowmapDescriptorSet;
	descriptorWrites[2].dstBinding = 2;
	descriptorWrites[2].dstArrayElement = 0;
	descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[2].descriptorCount = 1;
	descriptorWrites[2].pBufferInfo = &pointLightsDescriptorBufferInfo;



	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);


	return true;
}

bool createShadowmapResources(VkCommandPool graphicsCommandPool, VkQueue graphicsQueue, VkPhysicalDevice physicalDevice, VkDevice device,
							  VkSampler* pShadowmapSampler, VkRenderPass* pDirSpotShadowmapRenderPass, VkRenderPass* pPointShadowmapRenderPass,
							  VkExtent2D shadowmapExtent, uint32_t numDirLights, vector<Image>* pDirShadowmaps, uint32_t numSpotLights, vector<Image>* pSpotShadowmaps,
							  uint32_t numPointLights, vector<Image>* pPointShadowmaps) {
	if (pShadowmapSampler == nullptr || pDirSpotShadowmapRenderPass == nullptr || pPointShadowmapRenderPass == nullptr ||
		pDirShadowmaps == nullptr || pSpotShadowmaps == nullptr || pPointShadowmaps == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}
	VkFormat shadowmapImageFormat = VK_FORMAT_D32_SFLOAT;
	

	// Sampler
	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.anisotropyEnable = VK_FALSE;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 1.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	

	if (vkCreateSampler(device, &samplerCreateInfo, nullptr, pShadowmapSampler) != VK_SUCCESS) {
		cerr << "failed to create sampler!" << endl;
		return false;
	}


	// Render Pass
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = shadowmapImageFormat;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 0;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 0;
	subpass.pColorAttachments = nullptr;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	subpass.pResolveAttachments = nullptr;
	
	
	array<VkSubpassDependency, 2> subpassDependencies{};
	subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependencies[0].dstSubpass = 0;
	subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	subpassDependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	subpassDependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	subpassDependencies[1].srcSubpass = 0;
	subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	subpassDependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	subpassDependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


	VkRenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &depthAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
	renderPassCreateInfo.pDependencies = subpassDependencies.data();

	if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, pDirSpotShadowmapRenderPass) != VK_SUCCESS) {
		cerr << "failed to create shadow map render pass!" << endl;
		return false;
	}


	const uint32_t viewMask = 0b00111111;
	const uint32_t correlationMask = 0b00111111;

	VkRenderPassMultiviewCreateInfo renderPassMultiviewCreateInfo{};
	renderPassMultiviewCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO;
	renderPassMultiviewCreateInfo.subpassCount = 1;
	renderPassMultiviewCreateInfo.pViewMasks = &viewMask;
	renderPassMultiviewCreateInfo.correlationMaskCount = 1;
	renderPassMultiviewCreateInfo.pCorrelationMasks = &correlationMask;

	renderPassCreateInfo.pNext = &renderPassMultiviewCreateInfo;
	if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, pPointShadowmapRenderPass) != VK_SUCCESS) {
		cerr << "failed to create shadow map render pass!" << endl;
		return false;
	}


	// DIRECTIONAL LIGHTS
	pDirShadowmaps->resize(numDirLights);

	for (uint32_t i = 0; i < numDirLights; i++){
		// Image and Image Views
		if (!createImage(0, shadowmapImageFormat, shadowmapExtent, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, physicalDevice, device, &(*pDirShadowmaps)[i].image, &(*pDirShadowmaps)[i].imageMemory)) {
			cerr << "failed to create image for shadow map!" << endl;
			return false;
		}
		if (!createImageView((*pDirShadowmaps)[i].image, VK_IMAGE_VIEW_TYPE_2D, shadowmapImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 1, device, &(*pDirShadowmaps)[i].imageView)) {
			cerr << "failed to create image view for shadow map!" << endl;
			return false;
		}
	}


	if (numDirLights > 0) {
		VkCommandBuffer oneTimeCommandBuffer;
		if (!beginOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
			cerr << "failed to begin one-time command buffer for shadow map layout transition!" << endl;
			return false;
		}


		vector<VkImageMemoryBarrier> imageMemoryBarriers(numDirLights);

		for (uint32_t i = 0; i < numDirLights; i++){
			imageMemoryBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarriers[i].srcAccessMask = 0;
			imageMemoryBarriers[i].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			imageMemoryBarriers[i].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageMemoryBarriers[i].newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			imageMemoryBarriers[i].image = (*pDirShadowmaps)[i].image;
			imageMemoryBarriers[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			imageMemoryBarriers[i].subresourceRange.levelCount = 1;
			imageMemoryBarriers[i].subresourceRange.baseMipLevel = 0;
			imageMemoryBarriers[i].subresourceRange.layerCount = 1;
			imageMemoryBarriers[i].subresourceRange.baseArrayLayer = 0;
		}


		vkCmdPipelineBarrier(
			oneTimeCommandBuffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			0,
			0, nullptr,
			0, nullptr,
			numDirLights, imageMemoryBarriers.data()
		);

	
		if (!endOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
			cerr << "failed to end one-time command buffer for shadow map layout transition!" << endl;
			return false;
		}
	}


	// Framebuffers
	for (uint32_t i = 0; i < numDirLights; i++){
		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = *pDirSpotShadowmapRenderPass;
		framebufferCreateInfo.layers = 1;
		framebufferCreateInfo.width = shadowmapExtent.width;
		framebufferCreateInfo.height = shadowmapExtent.height;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = &(*pDirShadowmaps)[i].imageView;
		
		if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &(*pDirShadowmaps)[i].framebuffer) != VK_SUCCESS) {
			cerr << "failed to create shadow map framebuffer!" << endl;
			return false;
		}
	}



	// SPOT LIGHTS
	pSpotShadowmaps->resize(numSpotLights);

	for (uint32_t i = 0; i < numSpotLights; i++){
		// Image and Image Views
		if (!createImage(0, shadowmapImageFormat, shadowmapExtent, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, physicalDevice, device, &(*pSpotShadowmaps)[i].image, &(*pSpotShadowmaps)[i].imageMemory)) {
			cerr << "failed to create image for shadow map!" << endl;
			return false;
		}
		if (!createImageView((*pSpotShadowmaps)[i].image, VK_IMAGE_VIEW_TYPE_2D, shadowmapImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 1, device, &(*pSpotShadowmaps)[i].imageView)) {
			cerr << "failed to create image view for shadow map!" << endl;
			return false;
		}
	}


	if (numSpotLights > 0) {
		VkCommandBuffer oneTimeCommandBuffer;
		if (!beginOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
			cerr << "failed to begin one-time command buffer for shadow map layout transition!" << endl;
			return false;
		}


		vector<VkImageMemoryBarrier> imageMemoryBarriers(numSpotLights);

		for (uint32_t i = 0; i < numSpotLights; i++){
			imageMemoryBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarriers[i].srcAccessMask = 0;
			imageMemoryBarriers[i].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			imageMemoryBarriers[i].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageMemoryBarriers[i].newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			imageMemoryBarriers[i].image = (*pSpotShadowmaps)[i].image;
			imageMemoryBarriers[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			imageMemoryBarriers[i].subresourceRange.levelCount = 1;
			imageMemoryBarriers[i].subresourceRange.baseMipLevel = 0;
			imageMemoryBarriers[i].subresourceRange.layerCount = 1;
			imageMemoryBarriers[i].subresourceRange.baseArrayLayer = 0;
		}


		vkCmdPipelineBarrier(
			oneTimeCommandBuffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			0,
			0, nullptr,
			0, nullptr,
			numSpotLights, imageMemoryBarriers.data()
		);

	
		if (!endOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
			cerr << "failed to end one-time command buffer for shadow map layout transition!" << endl;
			return false;
		}
	}


	// Framebuffers
	for (uint32_t i = 0; i < numSpotLights; i++){
		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = *pDirSpotShadowmapRenderPass;
		framebufferCreateInfo.layers = 1;
		framebufferCreateInfo.width = shadowmapExtent.width;
		framebufferCreateInfo.height = shadowmapExtent.height;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = &(*pSpotShadowmaps)[i].imageView;
		
		if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &(*pSpotShadowmaps)[i].framebuffer) != VK_SUCCESS) {
			cerr << "failed to create shadow map framebuffer!" << endl;
			return false;
		}
	}



	// POINT LIGHTS
	pPointShadowmaps->resize(numPointLights);

	for (uint32_t i = 0; i < numPointLights; i++){
		// Image and Image Views
		if (!createImage(VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, shadowmapImageFormat, shadowmapExtent, 1, 6, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, physicalDevice, device, &(*pPointShadowmaps)[i].image, &(*pPointShadowmaps)[i].imageMemory)) {
			cerr << "failed to create image for shadow map!" << endl;
			return false;
		}
		if (!createImageView((*pPointShadowmaps)[i].image, VK_IMAGE_VIEW_TYPE_CUBE, shadowmapImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 6, device, &(*pPointShadowmaps)[i].imageView)) {
			cerr << "failed to create image view for shadow map!" << endl;
			return false;
		}
	}


	if (numPointLights > 0) {
		VkCommandBuffer oneTimeCommandBuffer;
		if (!beginOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
			cerr << "failed to begin one-time command buffer for shadow map layout transition!" << endl;
			return false;
		}


		vector<VkImageMemoryBarrier> imageMemoryBarriers(numPointLights);

		for (uint32_t i = 0; i < numPointLights; i++){
			imageMemoryBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarriers[i].srcAccessMask = 0;
			imageMemoryBarriers[i].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			imageMemoryBarriers[i].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageMemoryBarriers[i].newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			imageMemoryBarriers[i].image = (*pPointShadowmaps)[i].image;
			imageMemoryBarriers[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			imageMemoryBarriers[i].subresourceRange.levelCount = 1;
			imageMemoryBarriers[i].subresourceRange.baseMipLevel = 0;
			imageMemoryBarriers[i].subresourceRange.layerCount = 6;
			imageMemoryBarriers[i].subresourceRange.baseArrayLayer = 0;
		}


		vkCmdPipelineBarrier(
			oneTimeCommandBuffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			0,
			0, nullptr,
			0, nullptr,
			numPointLights, imageMemoryBarriers.data()
		);

	
		if (!endOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
			cerr << "failed to end one-time command buffer for shadow map layout transition!" << endl;
			return false;
		}
	}


	// Framebuffers
	for (uint32_t i = 0; i < numPointLights; i++){
		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = *pPointShadowmapRenderPass;
		framebufferCreateInfo.layers = 1;
		framebufferCreateInfo.width = shadowmapExtent.width;
		framebufferCreateInfo.height = shadowmapExtent.height;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = &(*pPointShadowmaps)[i].imageView;
		
		if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &(*pPointShadowmaps)[i].framebuffer) != VK_SUCCESS) {
			cerr << "failed to create shadow map framebuffer!" << endl;
			return false;
		}
	}


	return true;
}


bool createLightModelsAndObjects(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkPhysicalDevice physicalDevice, VkDevice device,
								 vector<Light>* pDirLights, vector<Light>* pSpotLights, vector<Light>* pPointLights,
								 vector<Model>* pLightModels, vector<Object>* pDirLightObjects, vector<Object>* pSpotLightObjects, vector<Object>* pPointLightObjects) {
	if (pDirLights == nullptr || pSpotLights == nullptr || pPointLights == nullptr ||
		pLightModels == nullptr || pDirLightObjects == nullptr || pSpotLightObjects == nullptr || pPointLightObjects == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}

	Assimp::Importer importer;


	vector<Vertex> cylinderVertices;
	vector<uint32_t> cylinderIndices;
	if (!loadMesh(&importer, "models/cylinder.fbx", &cylinderVertices, &cylinderIndices)) {
		cerr << "failed to load cylinder model!" << endl;
		return false;
	}

	vector<Vertex> coneVertices;
	vector<uint32_t> coneIndices;
	if (!loadMesh(&importer, "models/cone.fbx", &coneVertices, &coneIndices)) {
		cerr << "failed to load cone model!" << endl;
		return false;
	}

	vector<Vertex> sphereVertices;
	vector<uint32_t> sphereIndices;
	if (!loadMesh(&importer, "models/sphere.fbx", &sphereVertices, &sphereIndices)) {
		cerr << "failed to load sphere model!" << endl;
		return false;
	}



	pLightModels->resize(3);
	//cylinder
	if (!createModel(graphicsQueue, graphicsCommandPool, physicalDevice, device, &cylinderVertices, &cylinderIndices, &(*pLightModels)[0])) {
		cerr << "failed to create cylinder model!" <<  endl;
		return false;
	}

	//cone
	if (!createModel(graphicsQueue, graphicsCommandPool, physicalDevice, device, &coneVertices, &coneIndices, &(*pLightModels)[1])) {
		cerr << "failed to create cone model!" <<  endl;
		return false;
	}

	//sphere
	if (!createModel(graphicsQueue, graphicsCommandPool, physicalDevice, device, &sphereVertices, &sphereIndices, &(*pLightModels)[2])) {
		cerr << "failed to create sphere model!" <<  endl;
		return false;
	}



	pDirLightObjects->resize(pDirLights->size());
	pSpotLightObjects->resize(pSpotLights->size());
	pPointLightObjects->resize(pPointLights->size());

	size_t lightIndex = 0;
	for (size_t lightIndex = 0; lightIndex < pDirLights->size(); lightIndex++){
		Light light = (*pDirLights)[lightIndex];

		(*pDirLightObjects)[lightIndex].render = true;
		(*pDirLightObjects)[lightIndex].update = true;
		(*pDirLightObjects)[lightIndex].transform.translation = light.pos;
		(*pDirLightObjects)[lightIndex].transform.rotation = XMFLOAT3(Deg2Rad * light.rot.x, Deg2Rad * light.rot.y, 0.0f);
		Transform transform = (*pDirLightObjects)[lightIndex].transform;

		(*pDirLightObjects)[lightIndex].W = TRS(transform, RS(transform));

		(*pDirLightObjects)[lightIndex].modelIndex = 0;
	}

	for (size_t lightIndex = 0; lightIndex < pSpotLights->size(); lightIndex++) {
		Light light = (*pSpotLights)[lightIndex];

		(*pSpotLightObjects)[lightIndex].render = true;
		(*pSpotLightObjects)[lightIndex].update = true;
		(*pSpotLightObjects)[lightIndex].transform.translation = light.pos;
		(*pSpotLightObjects)[lightIndex].transform.rotation = XMFLOAT3(Deg2Rad * light.rot.x, Deg2Rad * light.rot.y, 0.0f);
		Transform transform = (*pSpotLightObjects)[lightIndex].transform;

		(*pSpotLightObjects)[lightIndex].W = TRS(transform, RS(transform));

		(*pSpotLightObjects)[lightIndex].modelIndex = 1;
	}

	for (size_t lightIndex = 0; lightIndex < pPointLights->size(); lightIndex++) {
		Light light = (*pPointLights)[lightIndex];

		(*pPointLightObjects)[lightIndex].render = true;
		(*pPointLightObjects)[lightIndex].update = true;
		(*pPointLightObjects)[lightIndex].transform.translation = light.pos;
		(*pPointLightObjects)[lightIndex].transform.rotation = XMFLOAT3(Deg2Rad * light.rot.x, Deg2Rad * light.rot.y, 0.0f);
		Transform transform = (*pPointLightObjects)[lightIndex].transform;

		(*pPointLightObjects)[lightIndex].W = TRS(transform, RS(transform));

		(*pPointLightObjects)[lightIndex].modelIndex = 2;
	}


	return true;
}

bool createLightBuffers(VkExtent2D shadowmapExtent, VkPhysicalDevice physicalDevice, VkDevice device, Buffer* pNumLightsBuffer,
						uint32_t maxNumDirLights, vector<Light>* pDirLights, Buffer* pDirLightsBuffer, vector<XMMATRIX>* pDirLightVPMatrices, Buffer* pDirLightVPMatricesBuffer,
						uint32_t maxNumSpotLights, vector<Light>* pSpotLights, Buffer* pSpotLightsBuffer, vector<XMMATRIX>* pSpotLightVPMatrices, Buffer* pSpotLightVPMatricesBuffer,
						uint32_t maxNumPointLights, vector<Light>* pPointLights, Buffer* pPointLightsBuffer, vector<XMMATRIX>* pPointLightVPMatrices, Buffer* pPointLightVPMatricesBuffer) {
	if (pNumLightsBuffer == nullptr ||
		pDirLights == nullptr || pDirLightsBuffer == nullptr || pDirLightVPMatrices == nullptr || pDirLightVPMatricesBuffer == nullptr ||
		pSpotLights == nullptr || pSpotLightsBuffer == nullptr || pSpotLightVPMatrices == nullptr || pSpotLightVPMatricesBuffer == nullptr ||
		pPointLights == nullptr || pPointLightsBuffer == nullptr || pPointLightVPMatrices == nullptr || pPointLightVPMatricesBuffer == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}


	uint32_t numDirLights = 0;
	if (numDirLights >= maxNumDirLights) {
		cerr << "number of directional lights (" << numDirLights << ") is greater than maximum (" << maxNumDirLights << ")!" << endl;
		return false;
	}
	pDirLights->resize(numDirLights);
	pDirLightVPMatrices->resize(numDirLights);
	//(*pDirLights)[0] = {{0.0f, 4.0f, 4.0f}, getDir(15.0f, -180.0f), {15.0f, -180.0f}, {0.63f, 0.25f, 0.16f, 1.0f}, 0.8f};
	//(*pDirLights)[1] = {{0.0f, 0.0f, 0.0f}, getDir(15.0f, -100.0f), {15.0f, -100.0f}, {0.63f, 0.25f, 0.16f, 1.0f}, 0.5f};

	XMMATRIX ortho_P = XMMatrixOrthographicLH(20.0f, 20.0f, 0.1f, 1000.0f);
	for (size_t i = 0; i < pDirLights->size(); i++) {
		XMMATRIX light_V = getViewMatrixLight((*pDirLights)[i].pos, (*pDirLights)[i].dir);
		(*pDirLightVPMatrices)[i] = XMMatrixMultiply(light_V, ortho_P);
	}



	uint32_t numSpotLights = 1;
	if (numSpotLights >= maxNumSpotLights) {
		cerr << "number of spot lights (" << numSpotLights << ") is greater than maximum (" << maxNumSpotLights << ")!" << endl;
		return false;
	}
	pSpotLights->resize(numSpotLights);
	pSpotLightVPMatrices->resize(numSpotLights);
	(*pSpotLights)[0] = {{-2.0f, 3.0f, 2.0f}, getDir(45.0f, -225.0f), {45.0f, -225.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, 3.0f};

	XMMATRIX pers_P = XMMatrixPerspectiveFovLH(90.0f * Deg2Rad, (static_cast<float>(shadowmapExtent.width) / static_cast<float>(shadowmapExtent.height)), 0.1f, 1000.0f);
	for (size_t i = 0; i < pSpotLights->size(); i++) {
		XMMATRIX light_V = getViewMatrixLight((*pSpotLights)[i].pos, (*pSpotLights)[i].dir);
		(*pSpotLightVPMatrices)[i] = XMMatrixMultiply(light_V, pers_P);
	}



	uint32_t numPointLights = 1;
	if (numPointLights >= maxNumPointLights) {
		cerr << "number of point lights (" << numPointLights << ") is greater than maximum (" << maxNumPointLights << ")!" << endl;
		return false;
	}
	pPointLights->resize(numPointLights);
	pPointLightVPMatrices->resize(numPointLights * 6);
	(*pPointLights)[0] = {{0.0f, 3.0f, 0.0f}, getDir(0.0f, 0.0f), {0.0f, 0.0f}, {0.93f, 0.55f, 0.16f, 1.0f}, 2.0f};
	//(*pPointLights)[1] = {{0.0f, 3.0f, 5.0f},  getDir(0.0f, 0.0f), {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, 1.0f};

	for (size_t i = 0; i < pPointLightVPMatrices->size(); i+=6) {
		(*pPointLightVPMatrices)[i + 0] = XMMatrixMultiply(getViewMatrixPointLight((*pPointLights)[i/6].pos, {-1.0f,  0.0f,  0.0f}, {0.0f, -1.0f,  0.0f}), pers_P);
		(*pPointLightVPMatrices)[i + 1] = XMMatrixMultiply(getViewMatrixPointLight((*pPointLights)[i/6].pos, { 1.0f,  0.0f,  0.0f}, {0.0f, -1.0f,  0.0f}), pers_P);
		(*pPointLightVPMatrices)[i + 2] = XMMatrixMultiply(getViewMatrixPointLight((*pPointLights)[i/6].pos, { 0.0f,  1.0f,  0.0f}, {0.0f,  0.0f,  1.0f}), pers_P);
		(*pPointLightVPMatrices)[i + 3] = XMMatrixMultiply(getViewMatrixPointLight((*pPointLights)[i/6].pos, { 0.0f, -1.0f,  0.0f}, {0.0f,  0.0f, -1.0f}), pers_P);
		(*pPointLightVPMatrices)[i + 4] = XMMatrixMultiply(getViewMatrixPointLight((*pPointLights)[i/6].pos, { 0.0f,  0.0f,  1.0f}, {0.0f, -1.0f,  0.0f}), pers_P);
		(*pPointLightVPMatrices)[i + 5] = XMMatrixMultiply(getViewMatrixPointLight((*pPointLights)[i/6].pos, { 0.0f,  0.0f, -1.0f}, {0.0f, -1.0f,  0.0f}), pers_P);
	}




	pNumLightsBuffer->bufferSize = static_cast<uint64_t>(sizeof(uint32_t) * 3);

	if (!createStaticBuffer(pNumLightsBuffer->bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
						    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
						    physicalDevice, device, &pNumLightsBuffer->buffer, &pNumLightsBuffer->bufferMemory)){
		cerr << "failed to create CPU numlLights buffer!" << endl;
		return false;
	}


	uint32_t* data;
	vkMapMemory(device, pNumLightsBuffer->bufferMemory, 0, pNumLightsBuffer->bufferSize, 0, (void**) &data);
	memcpy(data, (void*) &numDirLights, sizeof(uint32_t));
	memcpy(data + 1, (void*) &numSpotLights, sizeof(uint32_t));
	memcpy(data + 2, (void*) &numPointLights, sizeof(uint32_t));
	vkUnmapMemory(device, pNumLightsBuffer->bufferMemory);




	// DIRECTIONAL LIGHTS
	// assign buffer sizes
	if (!pDirLights->empty()) {
		pDirLightsBuffer->bufferSize = static_cast<uint64_t>(sizeof(Light) * pDirLights->size());
		pDirLightVPMatricesBuffer->bufferSize = static_cast<uint64_t>(sizeof(XMMATRIX) * pDirLightVPMatrices->size());
	}
	else {
		pDirLightsBuffer->bufferSize = 1;
		pDirLightVPMatricesBuffer->bufferSize = 1;
	}


	// create buffers
	if (!createStaticBuffer(pDirLightsBuffer->bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
						    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
						    physicalDevice, device, &pDirLightsBuffer->buffer, &pDirLightsBuffer->bufferMemory)){
		cerr << "failed to create CPU Directional Lights buffer!" << endl;
		return false;
	}
	if (!createStaticBuffer(pDirLightVPMatricesBuffer->bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		physicalDevice, device, &pDirLightVPMatricesBuffer->buffer, &pDirLightVPMatricesBuffer->bufferMemory)) {
		cerr << "failed to create CPU Directional Light VP Matrices buffer!" << endl;
		return false;
	}


	// copy data to buffers
	if (!pDirLights->empty()) {
		void* data;

		// copy light data to light buffer
		vkMapMemory(device, pDirLightsBuffer->bufferMemory, 0, pDirLightsBuffer->bufferSize, 0, &data);
		memcpy(data, (void*) pDirLights->data(), pDirLightsBuffer->bufferSize);
		vkUnmapMemory(device, pDirLightsBuffer->bufferMemory);

		// copy light vp matrices to light vp matrices buffer
		vkMapMemory(device, pDirLightVPMatricesBuffer->bufferMemory, 0, pDirLightVPMatricesBuffer->bufferSize, 0, &data);
		memcpy(data, (void*) pDirLightVPMatrices->data(), pDirLightVPMatricesBuffer->bufferSize);
		vkUnmapMemory(device, pDirLightVPMatricesBuffer->bufferMemory);
	}




	// SPOT LIGHTS
	// assign buffer sizes
	if (!pSpotLights->empty()) {
		pSpotLightsBuffer->bufferSize = static_cast<uint64_t>(sizeof(Light) * pSpotLights->size());
		pSpotLightVPMatricesBuffer->bufferSize = static_cast<uint64_t>(sizeof(XMMATRIX) * pSpotLightVPMatrices->size());
	}
	else {
		pSpotLightsBuffer->bufferSize = 1;
		pSpotLightVPMatricesBuffer->bufferSize = 1;
	}


	// create buffers
	if (!createStaticBuffer(pSpotLightsBuffer->bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
						    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
						    physicalDevice, device, &pSpotLightsBuffer->buffer, &pSpotLightsBuffer->bufferMemory)){
		cerr << "failed to create CPU Spot Lights buffer!" << endl;
		return false;
	}
	if (!createStaticBuffer(pSpotLightVPMatricesBuffer->bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		physicalDevice, device, &pSpotLightVPMatricesBuffer->buffer, &pSpotLightVPMatricesBuffer->bufferMemory)) {
		cerr << "failed to create CPU Spot Light VP Matrices buffer!" << endl;
		return false;
	}


	// copy data to buffers
	if (!pSpotLights->empty()) {
		void* data;

		// copy light data to light buffer
		vkMapMemory(device, pSpotLightsBuffer->bufferMemory, 0, pSpotLightsBuffer->bufferSize, 0, &data);
		memcpy(data, (void*) pSpotLights->data(), pSpotLightsBuffer->bufferSize);
		vkUnmapMemory(device, pSpotLightsBuffer->bufferMemory);

		// copy light vp matrices to light vp matrices buffer
		vkMapMemory(device, pSpotLightVPMatricesBuffer->bufferMemory, 0, pSpotLightVPMatricesBuffer->bufferSize, 0, &data);
		memcpy(data, (void*) pSpotLightVPMatrices->data(), pSpotLightVPMatricesBuffer->bufferSize);
		vkUnmapMemory(device, pSpotLightVPMatricesBuffer->bufferMemory);
	}




	// POINT LIGHTS
	// assign buffer sizes
	if (!pPointLights->empty()) {
		pPointLightsBuffer->bufferSize = static_cast<uint64_t>(sizeof(Light) * pPointLights->size());
		pPointLightVPMatricesBuffer->bufferSize = static_cast<uint64_t>(sizeof(XMMATRIX) * pPointLightVPMatrices->size());
	}
	else {
		pPointLightsBuffer->bufferSize = 1;
		pPointLightVPMatricesBuffer->bufferSize = 1;
	}


	// create buffers
	if (!createStaticBuffer(pPointLightsBuffer->bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
						    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
						    physicalDevice, device, &pPointLightsBuffer->buffer, &pPointLightsBuffer->bufferMemory)){
		cerr << "failed to create CPU Point Lights buffer!" << endl;
		return false;
	}
	if (!createStaticBuffer(pPointLightVPMatricesBuffer->bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		physicalDevice, device, &pPointLightVPMatricesBuffer->buffer, &pPointLightVPMatricesBuffer->bufferMemory)) {
		cerr << "failed to create CPU Point Light VP Matrices buffer!" << endl;
		return false;
	}


	// copy data to buffers
	if (!pPointLights->empty()) {
		void* data;

		// copy light data to light buffer
		vkMapMemory(device, pPointLightsBuffer->bufferMemory, 0, pPointLightsBuffer->bufferSize, 0, &data);
		memcpy(data, (void*) pPointLights->data(), pPointLightsBuffer->bufferSize);
		vkUnmapMemory(device, pPointLightsBuffer->bufferMemory);

		// copy light vp matrices to light vp matrices buffer
		vkMapMemory(device, pPointLightVPMatricesBuffer->bufferMemory, 0, pPointLightVPMatricesBuffer->bufferSize, 0, &data);
		memcpy(data, (void*) pPointLightVPMatrices->data(), pPointLightVPMatricesBuffer->bufferSize);
		vkUnmapMemory(device, pPointLightVPMatricesBuffer->bufferMemory);
	}


	return true;
}

XMFLOAT3 getDir(float rotX, float rotY) {
	XMFLOAT3 dir = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMVECTOR dirVec = XMLoadFloat3(&dir);
	XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(rotX * Deg2Rad, rotY * Deg2Rad, 0.0f);
	dirVec = XMVector3TransformCoord(dirVec, rotMat);
	XMStoreFloat3(&dir, dirVec);

	return dir;
}