#include "3_TexturesModelsAndObjects.h"


bool createModelsAndObjects(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkPhysicalDevice physicalDevice, VkDevice device, vector<Texture>* pAlbedomaps, vector<Model>* pModels, vector<Object>* pObjects) {
	if (pAlbedomaps == nullptr || pModels == nullptr || pObjects == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}

	// triangle
	const vector<Vertex> triVertices = {
		{{ 0.0f,  1.5f,  0.0f}, {0.0f, 0.0f, -1.0f}, {0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},	//0 TOP-MIDDLE
		{{ 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},	//1 BOTTOM-RIGHT
		{{-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},	//2 BOTTOM-LEFT
	};
	
	const vector<uint32_t> triIndices = {0, 1, 2};


	// quad
	const vector<Vertex> quadVertices = {
		{{-0.5f,  0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},	//0 TOP-LEFT
		{{ 0.5f,  0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},	//1 TOP-RIGHT
		{{ 0.5f,  0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},	//2 BOTTOM-RIGHT
		{{-0.5f,  0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}}		//3 BOTTOM-LEFT
	};
	
	const vector<uint32_t> quadIndices = {0, 1, 2, 2, 3, 0};


	// cube
	float scale = 0.5f;
	XMFLOAT3 front_bottomLeft =	 XMFLOAT3(-1.0f*scale, -1.0f*scale, -1.0f*scale);
	XMFLOAT3 front_bottomRight = XMFLOAT3( 1.0f*scale, -1.0f*scale, -1.0f*scale);
	XMFLOAT3 front_topLeft =	 XMFLOAT3(-1.0f*scale,  1.0f*scale, -1.0f*scale);
	XMFLOAT3 front_topRight =	 XMFLOAT3( 1.0f*scale,  1.0f*scale, -1.0f*scale);

	XMFLOAT3 back_bottomLeft =	 XMFLOAT3(-1.0f*scale, -1.0f*scale,  1.0f*scale);
	XMFLOAT3 back_bottomRight =  XMFLOAT3( 1.0f*scale, -1.0f*scale,  1.0f*scale);
	XMFLOAT3 back_topLeft =		 XMFLOAT3(-1.0f*scale,  1.0f*scale,  1.0f*scale);
	XMFLOAT3 back_topRight =	 XMFLOAT3( 1.0f*scale,  1.0f*scale,  1.0f*scale);


	XMFLOAT3 normal_front = XMFLOAT3(0.0f, 0.0f, -1.0f);
	XMFLOAT3 normal_back = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 normal_left = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	XMFLOAT3 normal_right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 normal_bottom = XMFLOAT3(0.0f, -1.0f, 0.0f);
	XMFLOAT3 normal_top = XMFLOAT3(0.0f, 1.0f, 0.0f);


	XMFLOAT2 uv_bottomLeft = XMFLOAT2(0.0f, 1.0f);
	XMFLOAT2 uv_bottomRight = XMFLOAT2(1.0f, 1.0f);
	XMFLOAT2 uv_topLeft = XMFLOAT2(0.0f, 0.0f);
	XMFLOAT2 uv_topRight = XMFLOAT2(1.0f, 0.0f);


	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 yellow = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);


	const vector<Vertex> cubeVertices = {
		{front_bottomLeft,  normal_front,  uv_bottomLeft , red   },
		{front_bottomRight, normal_front,  uv_bottomRight, green },
		{front_topLeft,     normal_front,  uv_topLeft    , blue  },
		{front_topRight,    normal_front,  uv_topRight   , yellow},

		{back_bottomRight,  normal_back,   uv_bottomLeft , red   },
		{back_bottomLeft,	normal_back,   uv_bottomRight, green },
		{back_topRight,     normal_back,   uv_topLeft    , blue  },
		{back_topLeft,		normal_back,   uv_topRight   , yellow},

		{back_bottomLeft,	normal_left,   uv_bottomLeft , green },
		{front_bottomLeft,	normal_left,   uv_bottomRight, red	 },
		{back_topLeft,		normal_left,   uv_topLeft    , yellow},
		{front_topLeft,		normal_left,   uv_topRight   , blue  },

		{front_bottomRight, normal_right,  uv_bottomLeft , green },
		{back_bottomRight,	normal_right,  uv_bottomRight, red	 },
		{front_topRight,    normal_right,  uv_topLeft    , yellow},
		{back_topRight,     normal_right,  uv_topRight   , blue  },

		{back_bottomLeft,	normal_bottom, uv_bottomLeft , green },
		{back_bottomRight,	normal_bottom, uv_bottomRight, red	 },
		{front_bottomLeft,  normal_bottom, uv_topLeft    , red	 },
		{front_bottomRight, normal_bottom, uv_topRight   , green },

		{front_topLeft,		normal_top,    uv_bottomLeft , blue  },
		{front_topRight,	normal_top,    uv_bottomRight, yellow},
		{back_topLeft,		normal_top,    uv_topLeft    , yellow},
		{back_topRight,		normal_top,    uv_topRight   , blue  }
	};
	
	const vector<uint32_t> cubeIndices = {
		 0,  2,  1,  1,  2,  3,
		 4,  6,  5,  5,  6,  7,
		 8, 10,  9,  9, 10, 11,
		12, 14, 13, 13, 14, 15,
		16, 18, 17, 17, 18, 19,
		20, 22, 21, 21, 22, 23
	};



	// Load custom models with assimp
	Assimp::Importer importer;

	vector<Vertex> roomVertices;
	vector<uint32_t> roomIndices;
	if (!loadMesh(&importer, "models/viking_room/viking_room.obj", &roomVertices, &roomIndices)) {
		cerr << "failed to load viking room model!" << endl;
		return false;
	}

	Texture albedomap;
	const char* albedomapFilename = "models/viking_room/viking_room-Albedo_BGRA32_Nvidia_Mips.ktx2";
	memcpy(albedomap.filename, albedomapFilename, strlen(albedomapFilename));
	if (!createTexture2D(graphicsQueue, graphicsCommandPool, physicalDevice, device, &albedomap)) {
		cerr << "failed to load viking room Albedomap!" << endl;
		return false;
	}

	pAlbedomaps->push_back(albedomap);


	vector<Vertex> bevelCubeVertices;
	vector<uint32_t> bevelCubeIndices;
	if (!loadMesh(&importer, "models/bevelcube.fbx", &bevelCubeVertices, &bevelCubeIndices)) {
		cerr << "failed to load bevel cube model!" << endl;
		return false;
	}


	



	pModels->resize(4);

	//quad
	if (!createModel(graphicsQueue, graphicsCommandPool, physicalDevice, device, &quadVertices, &quadIndices, &(*pModels)[0])) {
		cerr << "failed to create model : index - " << 0 << "!" <<  endl;
		return false;
	}

	//cube
	if (!createModel(graphicsQueue, graphicsCommandPool, physicalDevice, device, &cubeVertices, &cubeIndices, &(*pModels)[1])) {
		cerr << "failed to create model : index - " << 1 << "!" <<  endl;
		return false;
	}

	//room
	if (!createModel(graphicsQueue, graphicsCommandPool, physicalDevice, device, &roomVertices, &roomIndices, &(*pModels)[2])) {
		cerr << "failed to create model : index - " << 2 << "!" <<  endl;
		return false;
	}

	//bevel cube
	if (!createModel(graphicsQueue, graphicsCommandPool, physicalDevice, device, &bevelCubeVertices, &bevelCubeIndices, &(*pModels)[3])) {
		cerr << "failed to create model : index - " << 3 << "!" <<  endl;
		return false;
	}




	pObjects->resize(6);
	Transform transform;
	XMMATRIX RS_M;


	// floor
	(*pObjects)[0].render = true;
	(*pObjects)[0].update = false;
	(*pObjects)[0].transform.scale = XMFLOAT3(70.0f, 1.0f, 70.0f);
	transform = (*pObjects)[0].transform;
	RS_M = RS(transform);

	(*pObjects)[0].W = TRS(transform, RS_M);
	(*pObjects)[0].normalW = XMMatrixTranspose(XMMatrixInverse(nullptr, RS_M));

	(*pObjects)[0].modelIndex = 0;


	// skybox
	(*pObjects)[1].render = true;
	(*pObjects)[1].update = false;
	(*pObjects)[1].transform.scale = XMFLOAT3(1000.0f, 1000.0f, 1000.0f);
	transform = (*pObjects)[1].transform;
	RS_M = RS(transform);

	(*pObjects)[1].W = TRS(transform, RS_M);
	(*pObjects)[1].normalW = XMMatrixTranspose(XMMatrixInverse(nullptr, RS_M));

	(*pObjects)[1].modelIndex = 1;


	// center cube
	(*pObjects)[2].render = false;
	(*pObjects)[2].update = true;
	(*pObjects)[2].transform.scale = XMFLOAT3(2.0f, 2.0f, 2.0f);
	(*pObjects)[2].transform.translation = XMFLOAT3(0.0f, 1.001f, 0.0f);
	transform = (*pObjects)[2].transform;
	RS_M = RS(transform);

	(*pObjects)[2].W = TRS(transform, RS_M);
	(*pObjects)[2].normalW = XMMatrixTranspose(XMMatrixInverse(nullptr, RS_M));

	(*pObjects)[2].modelIndex = 1;
	(*pObjects)[2].textureIndex = 1;


	// flat wall
	(*pObjects)[3].render = false;
	(*pObjects)[3].update = false;
	(*pObjects)[3].transform.scale = XMFLOAT3(4.0f, 1.0f, 2.0f);
	(*pObjects)[3].transform.rotation = XMFLOAT3(Deg2Rad * 90.0f, 0.0f, 0.0f);
	(*pObjects)[3].transform.translation = XMFLOAT3(-2.0f, 1.001f, -5.0f);
	transform = (*pObjects)[3].transform;
	RS_M = RS(transform);
	
	(*pObjects)[3].W = TRS(transform, RS_M);
	(*pObjects)[3].normalW = XMMatrixTranspose(XMMatrixInverse(nullptr, RS_M));

	(*pObjects)[3].modelIndex = 0;
	(*pObjects)[3].textureIndex = 0;


	// room
	(*pObjects)[4].render = true;
	(*pObjects)[4].update = false;
	(*pObjects)[4].transform.scale = XMFLOAT3(7.0f, 7.0f, 7.0f);
	(*pObjects)[4].transform.rotation = XMFLOAT3(Deg2Rad * 90.0f, Deg2Rad * 0.0f, 0.0f);
	(*pObjects)[4].transform.translation = XMFLOAT3(0.0f, 0.601f, 0.0f);
	transform = (*pObjects)[4].transform;
	RS_M = RS(transform);
	
	(*pObjects)[4].W = TRS(transform, RS_M);
	(*pObjects)[4].normalW = XMMatrixTranspose(XMMatrixInverse(nullptr, RS_M));

	(*pObjects)[4].modelIndex = 2;
	(*pObjects)[4].textureIndex = 2;


	// bevel cube
	(*pObjects)[5].render = false;
	(*pObjects)[5].update = false;
	(*pObjects)[5].transform.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	//(*pObjects)[5].transform.rotation = XMFLOAT3(Deg2Rad * 90.0f, Deg2Rad * 0.0f, 0.0f);
	(*pObjects)[5].transform.translation = XMFLOAT3(0.0f, 2.0f, 2.0f);
	transform = (*pObjects)[5].transform;
	RS_M = RS(transform);

	(*pObjects)[5].W = TRS(transform, RS_M);
	(*pObjects)[5].normalW = XMMatrixTranspose(XMMatrixInverse(nullptr, RS_M));

	(*pObjects)[5].modelIndex = 3;
	(*pObjects)[5].textureIndex = 0;

	return true;
}


bool createTextures(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkPhysicalDevice physicalDevice, VkDevice device, Texture* pSkyboxTexture, vector<Texture>* pAlbedomaps) {
	//const char* Skybox_Filename = "textures/skyboxes/ColdSunset/ColdSunset_BC3_UNORM_PVR.ktx2";
	//const char* Skybox_Filename = "textures/skyboxes/DeepDusk/DeepDusk_BC3_UNORM_PVR.ktx2";
	const char* skyboxFilename = "textures/skyboxes/DeepDusk/DeepDusk_BGRA32.ktx2";
	memcpy(pSkyboxTexture->filename, skyboxFilename, strlen(skyboxFilename));
	if (!createTextureSkybox(graphicsQueue, graphicsCommandPool, physicalDevice, device, pSkyboxTexture)) {
		cerr << "failed to load Skybox texture!" << endl;
		return false;
	}


	pAlbedomaps->resize(2);

	const char* albedomapFilename = "textures/Rock-Albedo_BGRA32_Nvidia_Mips.ktx2";
	memcpy((*pAlbedomaps)[0].filename, albedomapFilename, strlen(albedomapFilename));
	if (!createTexture2D(graphicsQueue, graphicsCommandPool, physicalDevice, device, &(*pAlbedomaps)[0])) {
		cerr << "failed to load Albedo texture!" << endl;
		return false;
	}

	albedomapFilename = "textures/Moss-Albedo_BGRA32_Nvidia_Mips.ktx2";
	memcpy((*pAlbedomaps)[1].filename, albedomapFilename, strlen(albedomapFilename));
	if (!createTexture2D(graphicsQueue, graphicsCommandPool, physicalDevice, device, &(*pAlbedomaps)[1])) {
		cerr << "failed to load Albedo texture!" << endl;
		return false;
	}

	return true;
}

bool createTexture2D(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkPhysicalDevice physicalDevice, VkDevice device, Texture* p2DTexture) {
	VkFormat imageFormat = VK_FORMAT_B8G8R8A8_SRGB;

	
	// load image
	ktxTexture2* ktx_texture;
	if (ktxTexture_CreateFromNamedFile(p2DTexture->filename, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, (ktxTexture**) &ktx_texture) != KTX_SUCCESS) {
		cerr << "failed to load texture 2D!" << endl;
		return false;
	}
	VkDeviceSize imageBufferSize = static_cast<uint64_t>(ktxTexture_GetDataSize((ktxTexture*) ktx_texture));
	uint32_t width = ktx_texture->baseWidth;
	uint32_t height = ktx_texture->baseHeight;
	uint32_t mipLevels = ktx_texture->numLevels;

	
	// create staging buffer
	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
	
	if (!createStaticBuffer(imageBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
							VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
							physicalDevice, device, &stagingBuffer, &stagingBufferMemory)){
		ktxTexture_Destroy((ktxTexture*) ktx_texture);

		cerr << "failed to create staging buffer for texture 2D!" << endl;
		return false;
	}
	
	
	// copy image data to staging buffer
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, imageBufferSize, 0, &data);
	memcpy(data, (void*) ktxTexture_GetData((ktxTexture*) ktx_texture), imageBufferSize);
	vkUnmapMemory(device, stagingBufferMemory);
	
	
	// create image
	VkExtent2D imageExtent = { width, height };
	if (!createImage(0, imageFormat, imageExtent, mipLevels, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, physicalDevice, device, &p2DTexture->image, &p2DTexture->imageMemory)) {
		vkFreeMemory(device, stagingBufferMemory, nullptr);
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		ktxTexture_Destroy((ktxTexture*) ktx_texture);
		
		cerr << "failed to create image for texture 2D!" << endl;
		return false;
	}

	
	// image stuff
	VkCommandBuffer oneTimeCommandBuffer;
	if (!beginOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
		vkFreeMemory(device, stagingBufferMemory, nullptr);
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		ktxTexture_Destroy((ktxTexture*) ktx_texture);

		cerr << "failed to begin one-time command buffer for texture 2D layout transitions and buffer copy!" << endl;
		return false;
	}
	

	// transition from undefined layout to transfer dst layout
	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcAccessMask = 0;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = p2DTexture->image;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	
	vkCmdPipelineBarrier(
		oneTimeCommandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier
	);


	// copy buffer data to image
	vector<VkBufferImageCopy> copyRegions(mipLevels);

	for (uint32_t mipLevel = 0; mipLevel < mipLevels; mipLevel++){
		ktx_size_t offset;
		if (ktxTexture_GetImageOffset((ktxTexture*) ktx_texture, mipLevel, 0, 0, &offset) != KTX_SUCCESS) {
			vkQueueWaitIdle(graphicsQueue);
			vkFreeCommandBuffers(device, graphicsCommandPool, 1, &oneTimeCommandBuffer);

			vkFreeMemory(device, stagingBufferMemory, nullptr);
			vkDestroyBuffer(device, stagingBuffer, nullptr);
			ktxTexture_Destroy((ktxTexture*)ktx_texture);

			cerr << "failed to get image offset!" << endl;
			return false;
		}

		VkBufferImageCopy copyRegion{};
		copyRegion.bufferOffset = offset;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;
		copyRegion.imageOffset = {0, 0, 0};
		copyRegion.imageExtent = {width >> mipLevel, height >> mipLevel, 1};
		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = mipLevel;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;
		
		copyRegions[mipLevel] = copyRegion;
	}
	vkCmdCopyBufferToImage(oneTimeCommandBuffer, stagingBuffer, p2DTexture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(copyRegions.size()), copyRegions.data());
	
	
	// transition from transfer dst layout to shader read layout
	imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageMemoryBarrier.image = p2DTexture->image;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	
	vkCmdPipelineBarrier(
		oneTimeCommandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier
	);
	

	if (!endOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
		vkFreeMemory(device, stagingBufferMemory, nullptr);
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		ktxTexture_Destroy((ktxTexture*)ktx_texture);

		cerr << "failed to end one-time command buffer for texture 2D layout transitions and buffer copy!" << endl;
		return false;
	}
	
	
	// free buffer
	vkFreeMemory(device, stagingBufferMemory, nullptr);
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	ktxTexture_Destroy((ktxTexture*) ktx_texture);
	
	
	// create image view
	if (!createImageView(p2DTexture->image, VK_IMAGE_VIEW_TYPE_2D, imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 1, device, &p2DTexture->imageView)) {
		cerr << "failed to create image view for texture 2D!" << endl;
		return false;
	}

	return true;
}

bool createTextureSkybox(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkPhysicalDevice physicalDevice, VkDevice device, Texture* pSkyboxTexture) {
	//VkFormat imageFormat = VK_FORMAT_BC3_UNORM_BLOCK;
	VkFormat imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
	
	
	// load image
	ktxTexture2* ktx_texture;
	if (ktxTexture_CreateFromNamedFile(pSkyboxTexture->filename, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, (ktxTexture**) &ktx_texture) != KTX_SUCCESS) {
		cerr << "failed to load texture skybox!" << endl;
		return false;
	}
	VkDeviceSize imageBufferSize = static_cast<uint64_t>(ktxTexture_GetDataSize((ktxTexture*)ktx_texture));
	uint32_t width = ktx_texture->baseWidth;
	uint32_t height = ktx_texture->baseHeight;


	// create staging buffer
	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

	if (!createStaticBuffer(imageBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
						    physicalDevice, device, &stagingBuffer, &stagingBufferMemory)){
		ktxTexture_Destroy((ktxTexture*) ktx_texture);

		cerr << "failed to create staging buffer for texture skybox!" << endl;
		return false;
	}


	// copy image data to staging buffer
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, imageBufferSize, 0, &data);
	memcpy(data, (void*)ktxTexture_GetData((ktxTexture*) ktx_texture), imageBufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	
	// create image
	VkExtent2D imageExtent = { width, height };
	if (!createImage(VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, imageFormat, imageExtent, 1, 6, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, physicalDevice, device, &pSkyboxTexture->image, &pSkyboxTexture->imageMemory)) {
		vkFreeMemory(device, stagingBufferMemory, nullptr);
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		ktxTexture_Destroy((ktxTexture*) ktx_texture);

		cerr << "failed to create image for texture Skybox!" << endl;
		return false;
	}
	
	
	// image stuff
	VkCommandBuffer oneTimeCommandBuffer;
	if (!beginOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
		vkFreeMemory(device, stagingBufferMemory, nullptr);
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		ktxTexture_Destroy((ktxTexture*) ktx_texture);

		cerr << "failed to begin one-time command buffer for texture Skybox layout transitions and buffer copy!" << endl;
		return false;
	}
	

	// transition from undefined layout to transfer dst layout
	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcAccessMask = 0;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = pSkyboxTexture->image;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 6;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	
	vkCmdPipelineBarrier(
		oneTimeCommandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier
	);


	// copy buffer data to image
	vector<VkBufferImageCopy> copyRegions(6);

	for (uint32_t arrayLayer = 0; arrayLayer < 6; arrayLayer++) {
		ktx_size_t offset;
		if (ktxTexture_GetImageOffset((ktxTexture*) ktx_texture, 0, 0, arrayLayer, &offset) != KTX_SUCCESS) {
			vkQueueWaitIdle(graphicsQueue);
			vkFreeCommandBuffers(device, graphicsCommandPool, 1, &oneTimeCommandBuffer);

			vkFreeMemory(device, stagingBufferMemory, nullptr);
			vkDestroyBuffer(device, stagingBuffer, nullptr);
			ktxTexture_Destroy((ktxTexture*) ktx_texture);

			cerr << "failed to get image offset!" << endl;
			return false;
		}
		
		
		VkBufferImageCopy copyRegion{};
		copyRegion.bufferOffset = offset;
		copyRegion.imageSubresource.baseArrayLayer = arrayLayer;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;
		copyRegion.imageOffset = {0, 0, 0};
		copyRegion.imageExtent = {width, height, 1};
		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.layerCount = 1;

		copyRegions[arrayLayer] = copyRegion;
	}

	vkCmdCopyBufferToImage(oneTimeCommandBuffer, stagingBuffer, pSkyboxTexture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 6, copyRegions.data());
	
	
	// transition from transfer dst layout to shader read layout
	imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageMemoryBarrier.image = pSkyboxTexture->image;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 6;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	
	vkCmdPipelineBarrier(
		oneTimeCommandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier
	);
	

	if (!endOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
		vkFreeMemory(device, stagingBufferMemory, nullptr);
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		ktxTexture_Destroy((ktxTexture*) ktx_texture);

		cerr << "failed to end one-time command buffer for texture Skybox layout transitions and buffer copy!" << endl;
		return false;
	}


	// free buffer
	vkFreeMemory(device, stagingBufferMemory, nullptr);
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	ktxTexture_Destroy((ktxTexture*) ktx_texture);


	// create image view
	if (!createImageView(pSkyboxTexture->image, VK_IMAGE_VIEW_TYPE_CUBE, imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, 6, device, &pSkyboxTexture->imageView)) {
		cerr << "failed to create image view for texture Skybox!" << endl;
		return false;
	}

	return true;
}