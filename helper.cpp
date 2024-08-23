#include "main.h"


// helper functions
bool createShaderModule(VkDevice device, const char* filename, VkShaderModule* pShaderModule) {
	vector<char> byteCode;

	if (!loadByteCode(filename, &byteCode)) {
		cerr << "failed to load byte code for shader module!" << endl;
		return false;
	}


	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = byteCode.size();
	createInfo.pCode = reinterpret_cast<uint32_t*>(byteCode.data());
	if (vkCreateShaderModule(device, &createInfo, nullptr, pShaderModule) != VK_SUCCESS) {
		cerr << "failed to create shader module!" << endl;
		return false;
	}

	return true;
}

bool loadByteCode(const char* filename, vector<char>* pByteCode) {
	ifstream ifs;
	ifs.open(filename, ios::ate | ios::binary);
	if (!ifs.is_open()) {
		cerr << "failed to load shader file!" << endl;
		return false;
	}


	size_t fileSize = ifs.tellg();
	pByteCode->resize(fileSize);

	ifs.seekg(0);
	ifs.read(pByteCode->data(), fileSize);


	ifs.close();

	return true;
}


XMMATRIX getViewMatrixPointLight(XMFLOAT3 lightPosition, XMFLOAT3 focusDirectionF3, XMFLOAT3 upDirectionF3) {
	XMVECTOR eyePosition = XMLoadFloat3(&lightPosition);

	XMVECTOR focusDirection = XMLoadFloat3(&focusDirectionF3);

	XMVECTOR upDirection = XMLoadFloat3(&upDirectionF3);


	XMVECTOR focusPosition = XMVectorAdd(focusDirection, eyePosition);	// focusDirection -> focusPosition
	return XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
}

XMMATRIX getViewMatrixLight(XMFLOAT3 lightPosition, XMFLOAT3 lightDirection) {
	XMVECTOR eyePosition = XMLoadFloat3(&lightPosition);

	XMVECTOR focusDirection = XMLoadFloat3(&lightDirection);

	XMFLOAT3 upDirectionF3 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMVECTOR upDirection = XMLoadFloat3(&upDirectionF3);


	XMVECTOR focusPosition = XMVectorAdd(focusDirection, eyePosition);	// focusDirection -> focusPosition
	return XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
}


bool loadMesh(Assimp::Importer* pImporter, const char* filename, vector<Vertex>* pVertices, vector<uint32_t>* pIndices) {
	const aiScene* scene = pImporter->ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_MakeLeftHanded);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cerr << "ERROR::ASSIMP::" << pImporter->GetErrorString() << endl;
		return false;
	}

	aiNode* rootNode = scene->mRootNode;
	aiMesh* mesh = scene->mMeshes[rootNode->mChildren[0]->mMeshes[0]];

	for (size_t i = 0; i < mesh->mNumVertices; i++){
		Vertex vertex;
	
	
		vertex.pos = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.normal = XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		
		if (mesh->mTextureCoords[0]) {
			vertex.uv = XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		vertex.col = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	
	
		pVertices->push_back(vertex);
	}
	
	for (size_t i = 0; i < mesh->mNumFaces; i++){
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++){
			pIndices->push_back(face.mIndices[j]);
		}
	}

	return true;
}

bool createModel(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkPhysicalDevice physicalDevice, VkDevice device, const vector<Vertex>* pVertices, const vector<uint32_t>* pIndices, Model* pModel) {
	pModel->vertexCount = static_cast<uint32_t>(pVertices->size());
	pModel->indexCount = static_cast<uint32_t>(pIndices->size());

	pModel->vertexBuffer.bufferSize = static_cast<uint64_t>(sizeof(Vertex) * pVertices->size());
	pModel->indexBuffer.bufferSize = static_cast<uint64_t>(sizeof(uint32_t) * pIndices->size());


	// create staging buffers
	VkBuffer vertexStagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vertexStagingBufferMemory = VK_NULL_HANDLE;
	VkBuffer indexStagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory indexStagingBufferMemory = VK_NULL_HANDLE;

	if (!createStaticBuffer(pModel->vertexBuffer.bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
						   physicalDevice, device, &vertexStagingBuffer, &vertexStagingBufferMemory)){
		cerr << "failed to create model vertex staging buffer!" << endl;
		return false;
	}

	if (!createStaticBuffer(pModel->indexBuffer.bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
						   physicalDevice, device, &indexStagingBuffer, &indexStagingBufferMemory)){
		vkFreeMemory(device, vertexStagingBufferMemory, nullptr);
		vkDestroyBuffer(device, vertexStagingBuffer, nullptr);

		cerr << "failed to create model index staging buffer!" << endl;
		return false;
	}
	

	// copy data to staging buffers
	void* data;
	vkMapMemory(device, vertexStagingBufferMemory, 0, pModel->vertexBuffer.bufferSize, 0, &data);
	memcpy(data, (void*) pVertices->data(), pModel->vertexBuffer.bufferSize);
	vkUnmapMemory(device, vertexStagingBufferMemory);

	vkMapMemory(device, indexStagingBufferMemory, 0, pModel->indexBuffer.bufferSize, 0, &data);
	memcpy(data, (void*) pIndices->data(), pModel->indexBuffer.bufferSize);
	vkUnmapMemory(device, indexStagingBufferMemory);


	// create gpu buffers
	if (!createStaticBuffer(pModel->vertexBuffer.bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
							VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
							physicalDevice, device, &pModel->vertexBuffer.buffer, &pModel->vertexBuffer.bufferMemory)){
		vkFreeMemory(device, indexStagingBufferMemory, nullptr);
		vkDestroyBuffer(device, indexStagingBuffer, nullptr);
		vkFreeMemory(device, vertexStagingBufferMemory, nullptr);
		vkDestroyBuffer(device, vertexStagingBuffer, nullptr);

		cerr << "failed to create model vertex buffer!" << endl;
		return false;
	}

	if (!createStaticBuffer(pModel->indexBuffer.bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
							VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
							physicalDevice, device, &pModel->indexBuffer.buffer, &pModel->indexBuffer.bufferMemory)){
		vkFreeMemory(device, indexStagingBufferMemory, nullptr);
		vkDestroyBuffer(device, indexStagingBuffer, nullptr);
		vkFreeMemory(device, vertexStagingBufferMemory, nullptr);
		vkDestroyBuffer(device, vertexStagingBuffer, nullptr);

		cerr << "failed to create model index buffer!" << endl;
		return false;
	}


	// copy data from staging buffers to gpu buffers
	VkCommandBuffer oneTimeCommandBuffer;
	if (!beginOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
		cerr << "failed to begin one-time command buffer for model buffer copies!" << endl;
		return false;
	}
	
	VkBufferCopy copyRegion1{};
	copyRegion1.size = pModel->vertexBuffer.bufferSize;
	copyRegion1.srcOffset = 0;
	copyRegion1.dstOffset = 0;
	vkCmdCopyBuffer(oneTimeCommandBuffer, vertexStagingBuffer, pModel->vertexBuffer.buffer, 1, &copyRegion1);

	VkBufferCopy copyRegion2{};
	copyRegion2.size = pModel->indexBuffer.bufferSize;
	copyRegion2.srcOffset = 0;
	copyRegion2.dstOffset = 0;
	vkCmdCopyBuffer(oneTimeCommandBuffer, indexStagingBuffer, pModel->indexBuffer.buffer, 1, &copyRegion2);
	
	if (!endOneTimeCommandBuffer(graphicsQueue, graphicsCommandPool, device, &oneTimeCommandBuffer)) {
		cerr << "failed to end one-time command buffer for model buffer copies" << endl;
		return false;
	}


	// free staging buffers
	vkFreeMemory(device, indexStagingBufferMemory, nullptr);
	vkDestroyBuffer(device, indexStagingBuffer, nullptr);
	vkFreeMemory(device, vertexStagingBufferMemory, nullptr);
	vkDestroyBuffer(device, vertexStagingBuffer, nullptr);


	return true;
}


XMMATRIX TRS(Transform transform, XMMATRIX RS_M) {
	XMMATRIX Translation_M = XMMatrixTranslation(transform.translation.x, transform.translation.y, transform.translation.z);

	return XMMatrixMultiply(RS_M, Translation_M);
}

XMMATRIX RS(Transform transform) {
	XMMATRIX Scale_M = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);
	XMMATRIX Rotation_M = XMMatrixRotationRollPitchYaw(transform.rotation.x, transform.rotation.y, transform.rotation.z);

	return XMMatrixMultiply(Scale_M, Rotation_M);
}


bool endOneTimeCommandBuffer(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkDevice device, VkCommandBuffer* pOneTimeCommandBuffer) {
	if (vkEndCommandBuffer(*pOneTimeCommandBuffer) != VK_SUCCESS) {
		vkQueueWaitIdle(graphicsQueue);
		vkFreeCommandBuffers(device, graphicsCommandPool, 1, pOneTimeCommandBuffer);

		cerr << "failed to end recording one-time command to graphics command buffer!" << endl;
		return false;
	}

	// submit to queue
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = pOneTimeCommandBuffer;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		vkQueueWaitIdle(graphicsQueue);
		vkFreeCommandBuffers(device, graphicsCommandPool, 1, pOneTimeCommandBuffer);

		cerr << "failed to submit one-time command to graphics queue!" << endl;
		return false;
	}

	// cleanup
	vkQueueWaitIdle(graphicsQueue);
	vkFreeCommandBuffers(device, graphicsCommandPool, 1, pOneTimeCommandBuffer);

	return true;
}

bool beginOneTimeCommandBuffer(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkDevice device, VkCommandBuffer* pOneTimeCommandBuffer) {
	VkCommandBufferAllocateInfo cmdAllocInfo{};
	cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdAllocInfo.commandBufferCount = 1;
	cmdAllocInfo.commandPool = graphicsCommandPool;

	if (vkAllocateCommandBuffers(device, &cmdAllocInfo, pOneTimeCommandBuffer) != VK_SUCCESS) {
		cerr << "failed to allocate one-time command buffer!" << endl;
		return false;
	}


	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(*pOneTimeCommandBuffer, &beginInfo) != VK_SUCCESS) {
		vkQueueWaitIdle(graphicsQueue);
		vkFreeCommandBuffers(device, graphicsCommandPool, 1, pOneTimeCommandBuffer);

		cerr << "failed to begin recording one-time command to graphics command buffer!" << endl;
		return false;
	}


	return true;
}

bool createStaticBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags propertyFlags, VkPhysicalDevice physicalDevice, VkDevice device, VkBuffer* pBuffer, VkDeviceMemory* pBufferMemory) {
	VkBufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = bufferSize;
	createInfo.usage = bufferUsage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &createInfo, nullptr, pBuffer) != VK_SUCCESS) {
		cerr << "failed to create buffer!" << endl;
		return false;
	}


	VkMemoryRequirements memRequirements{};
	vkGetBufferMemoryRequirements(device, *pBuffer, &memRequirements);

	uint32_t memoryTypeIndex = 0;
	if (!findMemoryType(physicalDevice, memRequirements.memoryTypeBits, propertyFlags, &memoryTypeIndex)) {
		vkDestroyBuffer(device, *pBuffer, nullptr);
		*pBuffer = VK_NULL_HANDLE;

		cerr << "failed to find suitable memory type for buffer!" << endl;
		return false;
	}


	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = memoryTypeIndex;

	if (vkAllocateMemory(device, &allocInfo, nullptr, pBufferMemory) != VK_SUCCESS) {
		vkDestroyBuffer(device, *pBuffer, nullptr);
		*pBuffer = VK_NULL_HANDLE;

		cerr << "failed to allocate buffer memory!" << endl;
		return false;
	}

	if (vkBindBufferMemory(device, *pBuffer, *pBufferMemory, 0) != VK_SUCCESS) {
		vkFreeMemory(device, *pBufferMemory, nullptr);
		*pBufferMemory = VK_NULL_HANDLE;

		vkDestroyBuffer(device, *pBuffer, nullptr);
		*pBuffer = VK_NULL_HANDLE;


		cerr << "failed to bind buffer memory to buffer!" << endl;
		return false;
	}

	return true;
}

bool createImageView(VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspectMask, uint32_t mipLevels, uint32_t arrayLayers, VkDevice device, VkImageView* pImageView) {
	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = viewType;
	createInfo.format = format;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	createInfo.subresourceRange.aspectMask = aspectMask;
	createInfo.subresourceRange.levelCount = mipLevels;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.layerCount = arrayLayers;
	createInfo.subresourceRange.baseArrayLayer = 0;

	if (vkCreateImageView(device, &createInfo, nullptr, pImageView) != VK_SUCCESS) {
		cerr << "failed to create image view!" << endl;
		return false;
	}

	return true;
}

bool createImage(VkImageCreateFlags flags, VkFormat imageFormat, VkExtent2D imageExtent, uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits msaaSampleCount, VkImageUsageFlags imageUsage, VkPhysicalDevice physicalDevice, VkDevice device, VkImage* pImage, VkDeviceMemory* pImageMemory) {
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.flags = flags;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = imageFormat;
	imageCreateInfo.extent = {imageExtent.width, imageExtent.height, 1 };
	imageCreateInfo.mipLevels = mipLevels;
	imageCreateInfo.arrayLayers = arrayLayers;
	imageCreateInfo.samples = msaaSampleCount;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = imageUsage;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (vkCreateImage(device, &imageCreateInfo, nullptr, pImage) != VK_SUCCESS) {
		cerr << "failed to create image!" << endl;
		return false;
	}



	VkMemoryRequirements memRequirements{};
	vkGetImageMemoryRequirements(device, *pImage, &memRequirements);

	uint32_t memoryTypeIndex = 0;
	if (!findMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memoryTypeIndex)) {
		cerr << "failed to find suitable memory type for image!" << endl;
		return false;
	}


	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = memoryTypeIndex;

	if (vkAllocateMemory(device, &allocInfo, nullptr, pImageMemory) != VK_SUCCESS) {
		cerr << "failed to allocate image memory!" << endl;
		return false;
	}

	if (vkBindImageMemory(device, *pImage, *pImageMemory, 0) != VK_SUCCESS) {
		cerr << "failed to bind image memory to image!" << endl;
		return false;
	}

	return true;
}

bool findMemoryType(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags propertyFlags, uint32_t* pMemoryTypeIndex) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	uint32_t memoryTypeCount = memProperties.memoryTypeCount;

	for (uint32_t memoryTypeIndex = 0; memoryTypeIndex < memoryTypeCount; memoryTypeIndex++) {
		if ((memoryTypeBits & (1 << memoryTypeIndex)) && ((memProperties.memoryTypes[memoryTypeIndex].propertyFlags & propertyFlags) == propertyFlags)) {
			*pMemoryTypeIndex = memoryTypeIndex;
			return true;
		}
	}

	return false;
}