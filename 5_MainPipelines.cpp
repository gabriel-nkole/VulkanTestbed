#include "5_MainPipelines.h"


bool createGraphicsPipelines(VkExtent2D swapchainExtent, VkSampleCountFlagBits msaaSampleCount, VkRenderPass renderPass, vector<VkDescriptorSetLayout>* pDescriptorSetLayouts, VkDevice device,
							 VkPipelineLayout* pGraphicsPipelineLayout, VkPipelineLayout* pLightGraphicsPipelineLayout,
							 VkPipeline* pFloorGraphicsPipeline, VkPipeline* pSkyboxGraphicsPipeline, VkPipeline* pTextureGraphicsPipeline, VkPipeline* pLightGraphicsPipeline) {
	if (pDescriptorSetLayouts == nullptr ||
		pGraphicsPipelineLayout == nullptr || pLightGraphicsPipelineLayout == nullptr ||
		pFloorGraphicsPipeline == nullptr || pSkyboxGraphicsPipeline == nullptr || pTextureGraphicsPipeline == nullptr || pLightGraphicsPipeline == nullptr) {
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



	// floor
	VkShaderModule floor_vertexShaderModule = VK_NULL_HANDLE;
	if (!createShaderModule(device, "shaders/floor.vert.spv", &floor_vertexShaderModule)) {
		cerr << "failed to create shader module for floor vertex shader!" << endl;
		return false;
	}
	VkShaderModule floor_fragmentShaderModule = VK_NULL_HANDLE;
	if (!createShaderModule(device, "shaders/floor.frag.spv", &floor_fragmentShaderModule)) {
		vkDestroyShaderModule(device, floor_vertexShaderModule, nullptr);

		cerr << "failed to create shader module for floor fragment shader!" << endl;
		return false;
	}

	vertexShaderStageCreateInfo.module = floor_vertexShaderModule;
	fragmentShaderStageCreateInfo.module = floor_fragmentShaderModule;
	VkPipelineShaderStageCreateInfo floor_shaderStages[] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };


	// skybox
	VkShaderModule skybox_vertexShaderModule = VK_NULL_HANDLE;
	if (!createShaderModule(device, "shaders/skybox.vert.spv", &skybox_vertexShaderModule)) {
		vkDestroyShaderModule(device, floor_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_vertexShaderModule, nullptr);


		cerr << "failed to create shader module for skybox vertex shader!" << endl;
		return false;
	}
	VkShaderModule skybox_fragmentShaderModule = VK_NULL_HANDLE;
	if (!createShaderModule(device, "shaders/skybox.frag.spv", &skybox_fragmentShaderModule)) {
		vkDestroyShaderModule(device, skybox_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_vertexShaderModule, nullptr);

		cerr << "failed to create shader module for skybox fragment shader!" << endl;
		return false;
	}
	
	vertexShaderStageCreateInfo.module = skybox_vertexShaderModule;
	fragmentShaderStageCreateInfo.module = skybox_fragmentShaderModule;
	VkPipelineShaderStageCreateInfo skybox_shaderStages[] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };


	// texture
	VkShaderModule texture_vertexShaderModule = VK_NULL_HANDLE;
	if (!createShaderModule(device, "shaders/texture.vert.spv", &texture_vertexShaderModule)) {
		vkDestroyShaderModule(device, skybox_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, skybox_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_vertexShaderModule, nullptr);

		cerr << "failed to create shader module for texture vertex shader!" << endl;
		return false;
	}
	VkShaderModule texture_fragmentShaderModule = VK_NULL_HANDLE;
	if (!createShaderModule(device, "shaders/texture.frag.spv", &texture_fragmentShaderModule)) {
		vkDestroyShaderModule(device, texture_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, skybox_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, skybox_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_vertexShaderModule, nullptr);

		cerr << "failed to create shader module for texture fragment shader!" << endl;
		return false;
	}

	vertexShaderStageCreateInfo.module = texture_vertexShaderModule;
	fragmentShaderStageCreateInfo.module = texture_fragmentShaderModule;
	VkPipelineShaderStageCreateInfo texture_shaderStages[] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };


	// light
	VkShaderModule light_vertexShaderModule = VK_NULL_HANDLE;
	if (!createShaderModule(device, "shaders/light.vert.spv", &light_vertexShaderModule)) {
		vkDestroyShaderModule(device, texture_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, texture_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, skybox_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, skybox_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_vertexShaderModule, nullptr);

		cerr << "failed to create shader module for light vertex shader!" << endl;
		return false;
	}
	VkShaderModule light_fragmentShaderModule = VK_NULL_HANDLE;
	if (!createShaderModule(device, "shaders/light.frag.spv", &light_fragmentShaderModule)) {
		vkDestroyShaderModule(device, light_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, texture_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, texture_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, skybox_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, skybox_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_vertexShaderModule, nullptr);

		cerr << "failed to create shader module for light fragment shader!" << endl;
		return false;
	}

	vertexShaderStageCreateInfo.module = light_vertexShaderModule;
	fragmentShaderStageCreateInfo.module = light_fragmentShaderModule;
	VkPipelineShaderStageCreateInfo light_shaderStages[] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };



	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescription.stride = sizeof(Vertex);

	array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, Vertex::pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, Vertex::normal);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, Vertex::uv);

	attributeDescriptions[3].binding = 0;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[3].offset = offsetof(Vertex, Vertex::col);


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
	

	VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo{};
	

	VkViewport viewport;
	viewport.width = static_cast<float>(swapchainExtent.width);
	viewport.height = -static_cast<float>(swapchainExtent.height);
	viewport.x = 0.0f;
	viewport.y = static_cast<float>(swapchainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor;
	scissor.extent = swapchainExtent;
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
	multisampleStateCreateInfo.rasterizationSamples = msaaSampleCount;
	multisampleStateCreateInfo.sampleShadingEnable = VK_TRUE;
	multisampleStateCreateInfo.minSampleShading = 1.0f;
	multisampleStateCreateInfo.pSampleMask = nullptr;
	multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
	

	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
	depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
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
	colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; // WOWWWW

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
	

	VkDynamicState dynamicStates[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.dynamicStateCount = 2;
	dynamicStateCreateInfo.pDynamicStates = dynamicStates;


	array<VkPushConstantRange, 2> ranges{};
	ranges[0].offset = 0;
	ranges[0].size = sizeof(XMMATRIX)*3 + sizeof(uint32_t);
	ranges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	ranges[1].offset = 0;
	ranges[1].size = sizeof(XMMATRIX) + sizeof(uint32_t) + sizeof(uint32_t);
	ranges[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(pDescriptorSetLayouts->size());
	layoutCreateInfo.pSetLayouts = pDescriptorSetLayouts->data();
	layoutCreateInfo.pushConstantRangeCount = 1;
	layoutCreateInfo.pPushConstantRanges = &ranges[0];
	if (vkCreatePipelineLayout(device, &layoutCreateInfo, nullptr, pGraphicsPipelineLayout) != VK_SUCCESS) {
		vkDestroyShaderModule(device, light_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, light_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, texture_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, texture_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, skybox_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, skybox_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_vertexShaderModule, nullptr);
		
		cerr << "failed to create graphics pipeline layout!" << endl;
		return false;
	}

	layoutCreateInfo.pPushConstantRanges = &ranges[1];
	if (vkCreatePipelineLayout(device, &layoutCreateInfo, nullptr, pLightGraphicsPipelineLayout) != VK_SUCCESS) {
		vkDestroyShaderModule(device, light_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, light_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, texture_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, texture_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, skybox_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, skybox_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_vertexShaderModule, nullptr);
		
		cerr << "failed to create light graphics pipeline layout!" << endl;
		return false;
	}
	

	createInfo.stageCount = 2;
	createInfo.pStages = texture_shaderStages;
	createInfo.pVertexInputState = &vertexInputStateCreateInfo;
	createInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	createInfo.pTessellationState = nullptr;
	createInfo.pViewportState = &viewportStateCreateInfo;
	createInfo.pRasterizationState = &rasterizationStateCreateInfo;
	createInfo.pMultisampleState = &multisampleStateCreateInfo;
	createInfo.pDepthStencilState = &depthStencilStateCreateInfo;
	createInfo.pColorBlendState = &colorBlendStateCreateInfo;
	createInfo.pDynamicState = &dynamicStateCreateInfo;

	createInfo.layout = *pGraphicsPipelineLayout;
	createInfo.renderPass = renderPass;
	createInfo.subpass = 0;
	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = -1;

	VkGraphicsPipelineCreateInfo floor_createInfo = createInfo;
	floor_createInfo.pStages = floor_shaderStages;

	VkGraphicsPipelineCreateInfo skybox_createInfo = createInfo;
	skybox_createInfo.pStages = skybox_shaderStages;

	VkGraphicsPipelineCreateInfo texture_createInfo = createInfo;
	texture_createInfo.pStages = texture_shaderStages;

	VkGraphicsPipelineCreateInfo light_createInfo = createInfo;
	light_createInfo.pStages = light_shaderStages;




	array<VkGraphicsPipelineCreateInfo, 4> arr_createInfo = { floor_createInfo, skybox_createInfo, texture_createInfo, light_createInfo };
	array<VkPipeline, 4> graphicsPipelines{};
	
	if (vkCreateGraphicsPipelines(device, NULL, 4, arr_createInfo.data(), nullptr, graphicsPipelines.data()) != VK_SUCCESS) {
		vkDestroyShaderModule(device, light_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, light_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, texture_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, texture_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, skybox_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, skybox_vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, floor_vertexShaderModule, nullptr);

		cerr << "failed to create graphics pipelines!" << endl;
		return false;
	}
	memcpy(pFloorGraphicsPipeline, &graphicsPipelines[0], sizeof(VkPipeline));
	memcpy(pSkyboxGraphicsPipeline, &graphicsPipelines[1], sizeof(VkPipeline));
	memcpy(pTextureGraphicsPipeline, &graphicsPipelines[2], sizeof(VkPipeline));
	memcpy(pLightGraphicsPipeline, &graphicsPipelines[3], sizeof(VkPipeline));



	vkDestroyShaderModule(device, light_fragmentShaderModule, nullptr);
	vkDestroyShaderModule(device, light_vertexShaderModule, nullptr);
	vkDestroyShaderModule(device, texture_fragmentShaderModule, nullptr);
	vkDestroyShaderModule(device, texture_vertexShaderModule, nullptr);
	vkDestroyShaderModule(device, skybox_fragmentShaderModule, nullptr);
	vkDestroyShaderModule(device, skybox_vertexShaderModule, nullptr);
	vkDestroyShaderModule(device, floor_fragmentShaderModule, nullptr);
	vkDestroyShaderModule(device, floor_vertexShaderModule, nullptr);

	return true;
}

bool createDescriptorResources(VkSampler sampler, VkImageView skybox_ImageView, uint32_t maxNumAlbedomaps, uint32_t numAlbedomaps, vector<Texture>* pAlbedomaps,
							   uint32_t maxNumDirLights, uint32_t maxNumSpotLights, uint32_t maxNumPointLights, Buffer numLightsBuffer, VkSampler shadowmapSampler,
							   Buffer dirLightsBuffer, Buffer dirLightVPMatricesBuffer, uint32_t numDirLights, vector<Image>* pDirShadowmaps,
							   Buffer spotLightsBuffer, Buffer spotLightVPMatricesBuffer, uint32_t numSpotLights, vector<Image>* pSpotShadowmaps,
							   Buffer pointLightsBuffer, Buffer pointLightVPMatricesBuffer, uint32_t numPointLights, vector<Image>* pPointShadowmaps,
							   VkDevice device, vector<VkDescriptorSetLayout>* pDescriptorSetLayouts, VkDescriptorPool* pDescriptorPool, vector<VkDescriptorSet>* pDescriptorSets) {
	if (pAlbedomaps == nullptr ||
		pDirShadowmaps == nullptr || pSpotShadowmaps == nullptr || pPointShadowmaps == nullptr ||
		pDescriptorSetLayouts == nullptr || pDescriptorPool == nullptr || pDescriptorSets == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}


	// descriptor set layout
	pDescriptorSetLayouts->resize(2);
		
	array<VkDescriptorSetLayoutBinding, 2> layoutBindings0{};
	layoutBindings0[0].binding = 0;
	layoutBindings0[0].descriptorCount = 1;
	layoutBindings0[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	layoutBindings0[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	layoutBindings0[1].binding = 1;
	layoutBindings0[1].descriptorCount = maxNumAlbedomaps;
	layoutBindings0[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	layoutBindings0[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


	array<VkDescriptorSetLayoutBinding, 9> layoutBindings1{};
	layoutBindings1[0].binding = 0;
	layoutBindings1[0].descriptorCount = 1;
	layoutBindings1[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings1[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;

	layoutBindings1[1].binding = 1;
	layoutBindings1[1].descriptorCount = 1;
	layoutBindings1[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings1[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;

	layoutBindings1[2].binding = 2;
	layoutBindings1[2].descriptorCount = 1;
	layoutBindings1[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings1[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	layoutBindings1[3].binding = 3;
	layoutBindings1[3].descriptorCount = maxNumDirLights;
	layoutBindings1[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	layoutBindings1[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	layoutBindings1[4].binding = 4;
	layoutBindings1[4].descriptorCount = 1;
	layoutBindings1[4].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings1[4].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;

	layoutBindings1[5].binding = 5;
	layoutBindings1[5].descriptorCount = 1;
	layoutBindings1[5].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings1[5].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	layoutBindings1[6].binding = 6;
	layoutBindings1[6].descriptorCount = maxNumSpotLights;
	layoutBindings1[6].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	layoutBindings1[6].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	layoutBindings1[7].binding = 7;
	layoutBindings1[7].descriptorCount = 1;
	layoutBindings1[7].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings1[7].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;

	layoutBindings1[8].binding = 8;
	layoutBindings1[8].descriptorCount = maxNumPointLights;
	layoutBindings1[8].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	layoutBindings1[8].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings0.size());
	descriptorSetLayoutCreateInfo.pBindings = layoutBindings0.data();
	if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &(*pDescriptorSetLayouts)[0]) != VK_SUCCESS) {
		cerr << "failed to create descriptor set 0 layout!" << endl;
		return false;
	}

	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings1.size());
	descriptorSetLayoutCreateInfo.pBindings = layoutBindings1.data();
	if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &(*pDescriptorSetLayouts)[1]) != VK_SUCCESS) {
		cerr << "failed to create descriptor set 1 layout!" << endl;
		return false;
	}
	
	
	
	// descriptor pool
	array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[0].descriptorCount = (1 + maxNumAlbedomaps) +  (maxNumDirLights + maxNumSpotLights + maxNumPointLights);

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[1].descriptorCount = 0 + (1  +  2 + 2 + 1);
	
	
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolCreateInfo.maxSets = 2;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
	
	if (vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, pDescriptorPool) != VK_SUCCESS) {
		cerr << "failed to create descriptor pool!" << endl;
		return false;
	}
	
	
	
	// allocate descriptor sets
	pDescriptorSets->resize(2);
	
	VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
	descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocInfo.pSetLayouts = pDescriptorSetLayouts->data();
	descriptorSetAllocInfo.descriptorPool = *pDescriptorPool;
	descriptorSetAllocInfo.descriptorSetCount = 2;
	
	if (vkAllocateDescriptorSets(device, &descriptorSetAllocInfo, pDescriptorSets->data()) != VK_SUCCESS) {
		cerr << "failed to allocate descriptor sets!" << endl;
		return false;
	}



	// write to descriptor sets
	vector<VkWriteDescriptorSet> descriptorWrites0(1);


	VkDescriptorImageInfo skyboxDescriptorImageInfo{};
	skyboxDescriptorImageInfo.sampler = sampler;
	skyboxDescriptorImageInfo.imageView = skybox_ImageView;
	skyboxDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	descriptorWrites0[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites0[0].dstSet = (*pDescriptorSets)[0];
	descriptorWrites0[0].dstBinding = 0;
	descriptorWrites0[0].dstArrayElement = 0;
	descriptorWrites0[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites0[0].descriptorCount = 1;
	descriptorWrites0[0].pImageInfo = &skyboxDescriptorImageInfo;


	vector<VkDescriptorImageInfo> albedomapDescriptorImageInfos(numAlbedomaps);
	if (numAlbedomaps > 0) {
		for (uint32_t i = 0; i < numAlbedomaps; i++) {
			albedomapDescriptorImageInfos[i].sampler = sampler;
			albedomapDescriptorImageInfos[i].imageView = (*pAlbedomaps)[i].imageView;
			albedomapDescriptorImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		VkWriteDescriptorSet a{};
		a.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		a.dstSet = (*pDescriptorSets)[0];
		a.dstBinding = 1;
		a.dstArrayElement = 0;
		a.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		a.descriptorCount = numAlbedomaps;
		a.pImageInfo = albedomapDescriptorImageInfos.data();

		descriptorWrites0.push_back(a);
	}




	vector<VkWriteDescriptorSet> descriptorWrites1(3);

	VkDescriptorBufferInfo numLightsDescriptorBufferInfo{};
	numLightsDescriptorBufferInfo.buffer = numLightsBuffer.buffer;
	numLightsDescriptorBufferInfo.offset = 0;
	numLightsDescriptorBufferInfo.range = numLightsBuffer.bufferSize;

	descriptorWrites1[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites1[0].dstSet = (*pDescriptorSets)[1];
	descriptorWrites1[0].dstBinding = 0;
	descriptorWrites1[0].dstArrayElement = 0;
	descriptorWrites1[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites1[0].descriptorCount = 1;
	descriptorWrites1[0].pBufferInfo = &numLightsDescriptorBufferInfo;



	VkDescriptorBufferInfo dirLightsDescriptorBufferInfo{};
	dirLightsDescriptorBufferInfo.buffer = dirLightsBuffer.buffer;
	dirLightsDescriptorBufferInfo.offset = 0;
	dirLightsDescriptorBufferInfo.range = dirLightsBuffer.bufferSize;

	descriptorWrites1[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites1[1].dstSet = (*pDescriptorSets)[1];
	descriptorWrites1[1].dstBinding = 1;
	descriptorWrites1[1].dstArrayElement = 0;
	descriptorWrites1[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites1[1].descriptorCount = 1;
	descriptorWrites1[1].pBufferInfo = &dirLightsDescriptorBufferInfo;


	VkDescriptorBufferInfo dirLightVPMatricesDescriptorBufferInfo{};
	dirLightVPMatricesDescriptorBufferInfo.buffer = dirLightVPMatricesBuffer.buffer;
	dirLightVPMatricesDescriptorBufferInfo.offset = 0;
	dirLightVPMatricesDescriptorBufferInfo.range = dirLightVPMatricesBuffer.bufferSize;

	descriptorWrites1[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites1[2].dstSet = (*pDescriptorSets)[1];
	descriptorWrites1[2].dstBinding = 2;
	descriptorWrites1[2].dstArrayElement = 0;
	descriptorWrites1[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites1[2].descriptorCount = 1;
	descriptorWrites1[2].pBufferInfo = &dirLightVPMatricesDescriptorBufferInfo;


	vector<VkDescriptorImageInfo> dirShadowmapDescriptorImageInfos(numDirLights);
	if (numDirLights > 0) {
		for (uint32_t i = 0; i < numDirLights; i++){
			dirShadowmapDescriptorImageInfos[i].sampler = shadowmapSampler;
			dirShadowmapDescriptorImageInfos[i].imageView = (*pDirShadowmaps)[i].imageView;
			dirShadowmapDescriptorImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		}

		VkWriteDescriptorSet a{};
		a.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		a.dstSet = (*pDescriptorSets)[1];
		a.dstBinding = 3;
		a.dstArrayElement = 0;
		a.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		a.descriptorCount = numDirLights;
		a.pImageInfo = dirShadowmapDescriptorImageInfos.data();

		descriptorWrites1.push_back(a);
	}



	VkDescriptorBufferInfo spotLightsDescriptorBufferInfo{};
	spotLightsDescriptorBufferInfo.buffer = spotLightsBuffer.buffer;
	spotLightsDescriptorBufferInfo.offset = 0;
	spotLightsDescriptorBufferInfo.range = spotLightsBuffer.bufferSize;

	VkWriteDescriptorSet a{};
	a = {};
	a.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	a.dstSet = (*pDescriptorSets)[1];
	a.dstBinding = 4;
	a.dstArrayElement = 0;
	a.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	a.descriptorCount = 1;
	a.pBufferInfo = &spotLightsDescriptorBufferInfo;
	descriptorWrites1.push_back(a);


	VkDescriptorBufferInfo spotLightVPMatricesDescriptorBufferInfo{};
	spotLightVPMatricesDescriptorBufferInfo.buffer = spotLightVPMatricesBuffer.buffer;
	spotLightVPMatricesDescriptorBufferInfo.offset = 0;
	spotLightVPMatricesDescriptorBufferInfo.range = spotLightVPMatricesBuffer.bufferSize;

	a = {};
	a.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	a.dstSet = (*pDescriptorSets)[1];
	a.dstBinding = 5;
	a.dstArrayElement = 0;
	a.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	a.descriptorCount = 1;
	a.pBufferInfo = &spotLightVPMatricesDescriptorBufferInfo;
	descriptorWrites1.push_back(a);


	vector<VkDescriptorImageInfo> spotShadowmapDescriptorImageInfos(numSpotLights);
	if (numSpotLights > 0) {
		for (uint32_t i = 0; i < numSpotLights; i++){
			spotShadowmapDescriptorImageInfos[i].sampler = shadowmapSampler;
			spotShadowmapDescriptorImageInfos[i].imageView = (*pSpotShadowmaps)[i].imageView;
			spotShadowmapDescriptorImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		}

		VkWriteDescriptorSet a{};
		a = {};
		a.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		a.dstSet = (*pDescriptorSets)[1];
		a.dstBinding = 6;
		a.dstArrayElement = 0;
		a.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		a.descriptorCount = numSpotLights;
		a.pImageInfo = spotShadowmapDescriptorImageInfos.data();
		descriptorWrites1.push_back(a);
	}



	VkDescriptorBufferInfo pointLightsDescriptorBufferInfo{};
	pointLightsDescriptorBufferInfo.buffer = pointLightsBuffer.buffer;
	pointLightsDescriptorBufferInfo.offset = 0;
	pointLightsDescriptorBufferInfo.range = pointLightsBuffer.bufferSize;

	a = {};
	a.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	a.dstSet = (*pDescriptorSets)[1];
	a.dstBinding = 7;
	a.dstArrayElement = 0;
	a.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	a.descriptorCount = 1;
	a.pBufferInfo = &pointLightsDescriptorBufferInfo;
	descriptorWrites1.push_back(a);


	vector<VkDescriptorImageInfo> pointShadowmapDescriptorImageInfos(numPointLights);
	if (numPointLights > 0) {
		for (uint32_t i = 0; i < numPointLights; i++){
			pointShadowmapDescriptorImageInfos[i].sampler = shadowmapSampler;
			pointShadowmapDescriptorImageInfos[i].imageView = (*pPointShadowmaps)[i].imageView;
			pointShadowmapDescriptorImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		}

		VkWriteDescriptorSet a{};
		a = {};
		a.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		a.dstSet = (*pDescriptorSets)[1];
		a.dstBinding = 8;
		a.dstArrayElement = 0;
		a.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		a.descriptorCount = numPointLights;
		a.pImageInfo = pointShadowmapDescriptorImageInfos.data();

		descriptorWrites1.push_back(a);
	}


	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites0.size()), descriptorWrites0.data(), 0, nullptr);
	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites1.size()), descriptorWrites1.data(), 0, nullptr);


	return true;
}

bool createSampler(VkDevice device, VkSampler* pSampler) {
	if (pSampler == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}

	// create texture sampler
	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.anisotropyEnable = VK_TRUE;
	samplerCreateInfo.maxAnisotropy = 16.0f;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 10.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	

	if (vkCreateSampler(device, &samplerCreateInfo, nullptr, pSampler) != VK_SUCCESS) {
		cerr << "failed to create sampler!" << endl;
		return false;
	}

	return true;
}