#include "main.h"
#include "1_InstanceDeviceAndCommandPool.h"
#include "2_MainPipelinesFramebuffers.h"
#include "3_TexturesModelsAndObjects.h"
#include "4_LightsAndShadows.h"
#include "5_MainPipelines.h"


// CONSTANTS
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const VkSampleCountFlagBits MSAA_SAMPLE_COUNT = VK_SAMPLE_COUNT_4_BIT;

const uint32_t MAX_NUM_ALBEDOMAPS = 100;

const uint32_t MAX_NUM_DIR_LIGHTS = 100;
const uint32_t MAX_NUM_SPOT_LIGHTS = 100;
const uint32_t MAX_NUM_POINT_LIGHTS = 100;
const VkExtent2D SHADOWMAP_EXTENT = { 1024, 1024 };


// VALIDATION SUPPORT
#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
const bool ENABLE_VALIDATION_LAYERS = true;
#endif

vector<const char*> VALIDATION_LAYERS = {
	"VK_LAYER_KHRONOS_validation"
};


// DEVICE EXTENSION SUPPORT
vector<const char*> DEVICE_EXTENSIONS = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_MULTIVIEW_EXTENSION_NAME
};


// GLOBALS
GLFWwindow* Window = nullptr;

VkInstance Instance = VK_NULL_HANDLE;
VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
VkSurfaceKHR Surface = VK_NULL_HANDLE;
VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
Queue_Families QueueFamilies{};

VkDevice Device = VK_NULL_HANDLE;
VkQueue GraphicsQueue = VK_NULL_HANDLE;
VkQueue PresentQueue = VK_NULL_HANDLE;
VkCommandPool GraphicsCommandPool = VK_NULL_HANDLE;

VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
vector<VkImage> SwapchainImages;
VkFormat SwapchainFormat;
VkExtent2D SwapchainExtent;
vector<VkImageView> SwapchainImageViews;

VkImage ColorImage = VK_NULL_HANDLE;
VkDeviceMemory ColorImageMemory = VK_NULL_HANDLE;
VkImageView ColorImageView = VK_NULL_HANDLE;
VkFormat DepthImageFormat;
VkImage DepthImage = VK_NULL_HANDLE;
VkDeviceMemory DepthImageMemory = VK_NULL_HANDLE;
VkImageView DepthImageView = VK_NULL_HANDLE;

VkRenderPass RenderPass = VK_NULL_HANDLE;
vector<VkFramebuffer> SwapchainFramebuffers;

Texture SkyboxTexture{};
vector<Texture> Albedomaps;

vector<Model> Models;
vector<Object> Objects;

Buffer NumLightsBuffer;

vector<Light> DirLights;
Buffer DirLightsBuffer;
vector<XMMATRIX> DirLightVPMatrices;
Buffer DirLightVPMatricesBuffer;

vector<Light> SpotLights;
Buffer SpotLightsBuffer;
vector<XMMATRIX> SpotLightVPMatrices;
Buffer SpotLightVPMatricesBuffer;

vector<Light> PointLights;
Buffer PointLightsBuffer;
vector<XMMATRIX> PointLightVPMatrices;
Buffer PointLightVPMatricesBuffer;

vector<Model> LightModels;
vector<Object> DirLightObjects;
vector<Object> SpotLightObjects;
vector<Object> PointLightObjects;

VkSampler ShadowmapSampler = VK_NULL_HANDLE;
VkRenderPass DirSpotShadowmapRenderPass = VK_NULL_HANDLE;
VkRenderPass PointShadowmapRenderPass = VK_NULL_HANDLE;
vector<Image> DirShadowmaps;
vector<Image> SpotShadowmaps;
vector<Image> PointShadowmaps;

VkDescriptorSetLayout PointShadowmapDescriptorSetLayout;
VkDescriptorPool PointShadowmapDescriptorPool;
VkDescriptorSet PointShadowmapDescriptorSet;

VkPipelineLayout DirSpotShadowmapGraphicsPipelineLayout = VK_NULL_HANDLE;
VkPipelineLayout PointShadowmapGraphicsPipelineLayout = VK_NULL_HANDLE;
VkPipeline DirSpotShadowmapGraphicsPipeline = VK_NULL_HANDLE;
VkPipeline PointShadowmapGraphicsPipeline = VK_NULL_HANDLE;

VkSampler Sampler = VK_NULL_HANDLE;

vector<VkDescriptorSetLayout> DescriptorSetLayouts;
VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
vector<VkDescriptorSet> DescriptorSets;

VkPipelineLayout GraphicsPipelineLayout = VK_NULL_HANDLE;
VkPipelineLayout LightGraphicsPipelineLayout = VK_NULL_HANDLE;
VkPipeline FloorGraphicsPipeline = VK_NULL_HANDLE;
VkPipeline SkyboxGraphicsPipeline = VK_NULL_HANDLE;
VkPipeline TextureGraphicsPipeline = VK_NULL_HANDLE;
VkPipeline LightGraphicsPipeline = VK_NULL_HANDLE;

VkCommandBuffer DrawCommandBuffer = VK_NULL_HANDLE;

VkSemaphore ImageAcquiredSemaphore = VK_NULL_HANDLE;
VkSemaphore RenderCompleteSemaphore = VK_NULL_HANDLE;
VkFence FrameFence = VK_NULL_HANDLE;

chrono::steady_clock::time_point LastTime;
double LastCursorXPos = 0.0;
double LastCursorYPos = 0.0;
float DeltaCursorXPos = 0.0f;
float DeltaCursorYPos = 0.0f;
//XMFLOAT3 CameraRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
//XMFLOAT3 CameraPosition = XMFLOAT3(0.0f, 1.7f, -5.0f);
XMFLOAT3 CameraRotation = XMFLOAT3(25.0f, -110.0f, 0.0f);
XMFLOAT3 CameraPosition = XMFLOAT3(4.0f, 4.0f, 2.0f);

float RotationAngle = 0.0f;
XMMATRIX VP = XMMatrixIdentity();



// FUNCTIONS
// main loop functions
void mainLoop();

void drawFrame();

void recordDrawCommand(uint32_t imageIndex);
void drawIndexedPointLight(size_t pointLightObjectIndex);
void drawIndexedSpotLight(size_t spotLightObjectIndex);
void drawIndexedDirLight(size_t dirLightObjectIndex);
void drawIndexed(size_t objectIndex);
void drawIndexedPointShadow(size_t objectIndex, size_t pointLightIndex);
void drawIndexedSpotShadow(size_t objectIndex, size_t spotLightIndex);
void drawIndexedDirShadow(size_t objectIndex, size_t dirLightIndex);

void recreateSwapchain();


void update();
void updateLights(float frameTime);
XMMATRIX getViewMatrixCamera(XMFLOAT3 cameraPosition, XMFLOAT3 cameraRotation);
void updateCamera(float frameTime);



// init functions
void init();

void windowSizeCallback(GLFWwindow* window, int width, int height);
void cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

bool createDrawCommandBuffer(VkCommandPool graphicsCommandPool, VkDevice device, VkCommandBuffer* pDrawCommandBuffer);
bool createSyncObjects(VkDevice device, VkSemaphore* pImageAcquiredSemaphore, VkSemaphore* pRenderCompleteSemaphore, VkFence* pFrameFence);

void processResult(bool result, const char* errorMessage);



// cleanup functions
void cleanup();
void cleanupTexture(Texture* pTexture);
void cleanupBuffer(Buffer* pBuffer);
void cleanupImage(Image* pImage);




// MAIN
int main() {
	try {
		init();

		mainLoop(); // need to remove exception handling from here eventually 

		cleanup();
	}

	catch(const exception& ex){
		cerr << ex.what() << endl;
	}
}

void mainLoop() {
	LastTime = std::chrono::high_resolution_clock::now();
	glfwGetCursorPos(Window, &LastCursorXPos, &LastCursorYPos);

	while (!glfwWindowShouldClose(Window) && glfwGetKey(Window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
		glfwPollEvents();
		update();
		drawFrame();
	}

	vkDeviceWaitIdle(Device);
}


void drawFrame() {
	vkWaitForFences(Device, 1, &FrameFence, VK_TRUE, UINT64_MAX);


	// get new image
	uint32_t imageIndex = 0;
	VkResult result = vkAcquireNextImageKHR(Device, Swapchain, UINT64_MAX, ImageAcquiredSemaphore, VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		//cout << "mustResize" << endl;
		recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS) {
		cleanup();
		throw runtime_error("failed to acquire next image in swap chain!");
	}
	vkResetFences(Device, 1, &FrameFence);



	// submit draw command
	vkResetCommandBuffer(DrawCommandBuffer, 0);
	recordDrawCommand(imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	submitInfo.waitSemaphoreCount = 1;
	VkPipelineStageFlags waitDstStageMasks[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.pWaitDstStageMask = waitDstStageMasks;
	submitInfo.pWaitSemaphores = &ImageAcquiredSemaphore;

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &RenderCompleteSemaphore;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &DrawCommandBuffer;

	if (vkQueueSubmit(GraphicsQueue, 1, &submitInfo, FrameFence) != VK_SUCCESS) {
		cleanup();
		throw runtime_error("failed to submit draw command to graphics queue!");
	}


	// present image
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &Swapchain;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &RenderCompleteSemaphore;

	result = vkQueuePresentKHR(PresentQueue, &presentInfo);
}




void recordDrawCommand(uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(DrawCommandBuffer, &beginInfo) != VK_SUCCESS) {
		cleanup();
		throw runtime_error("failed to begin recording draw command to graphics command buffer!");
	}


	// create directional light shadow maps
	for (size_t dirLightIndex = 0; dirLightIndex < DirLights.size(); dirLightIndex++) {
		VkRenderPassBeginInfo DirSpotShadowmapRenderPassBegin{};
		DirSpotShadowmapRenderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		DirSpotShadowmapRenderPassBegin.renderPass = DirSpotShadowmapRenderPass;
		DirSpotShadowmapRenderPassBegin.renderArea.extent = SHADOWMAP_EXTENT;
		DirSpotShadowmapRenderPassBegin.renderArea.offset = { 0, 0 };
		DirSpotShadowmapRenderPassBegin.framebuffer = DirShadowmaps[dirLightIndex].framebuffer;

		VkClearValue clearValue{};
		clearValue.depthStencil = { 1.0f, 0 };
		DirSpotShadowmapRenderPassBegin.clearValueCount = 1;
		DirSpotShadowmapRenderPassBegin.pClearValues = &clearValue;

		vkCmdBeginRenderPass(DrawCommandBuffer, &DirSpotShadowmapRenderPassBegin, VK_SUBPASS_CONTENTS_INLINE);


		vkCmdBindPipeline(DrawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, DirSpotShadowmapGraphicsPipeline);
		drawIndexedDirShadow(0, dirLightIndex);

		for (size_t i = 2; i < Objects.size(); i++) {
			drawIndexedDirShadow(i, dirLightIndex);
		}


		vkCmdEndRenderPass(DrawCommandBuffer);
	}



	// create spot light shadow maps
	for (size_t spotLightIndex = 0; spotLightIndex < SpotLights.size(); spotLightIndex++) {
		VkRenderPassBeginInfo DirSpotShadowmapRenderPassBegin{};
		DirSpotShadowmapRenderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		DirSpotShadowmapRenderPassBegin.renderPass = DirSpotShadowmapRenderPass;
		DirSpotShadowmapRenderPassBegin.renderArea.extent = SHADOWMAP_EXTENT;
		DirSpotShadowmapRenderPassBegin.renderArea.offset = { 0, 0 };
		DirSpotShadowmapRenderPassBegin.framebuffer = SpotShadowmaps[spotLightIndex].framebuffer;

		VkClearValue clearValue{};
		clearValue.depthStencil = { 1.0f, 0 };
		DirSpotShadowmapRenderPassBegin.clearValueCount = 1;
		DirSpotShadowmapRenderPassBegin.pClearValues = &clearValue;

		vkCmdBeginRenderPass(DrawCommandBuffer, &DirSpotShadowmapRenderPassBegin, VK_SUBPASS_CONTENTS_INLINE);


		vkCmdBindPipeline(DrawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, DirSpotShadowmapGraphicsPipeline);
		drawIndexedSpotShadow(0, spotLightIndex);

		for (size_t i = 2; i < Objects.size(); i++) {
			drawIndexedSpotShadow(i, spotLightIndex);
		}


		vkCmdEndRenderPass(DrawCommandBuffer);
	}



	// create point light shadow maps
	for (size_t pointLightIndex = 0; pointLightIndex < PointLights.size(); pointLightIndex++) {
		VkRenderPassBeginInfo PointShadowmapRenderPassBegin{};
		PointShadowmapRenderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		PointShadowmapRenderPassBegin.renderPass = PointShadowmapRenderPass;
		PointShadowmapRenderPassBegin.renderArea.extent = SHADOWMAP_EXTENT;
		PointShadowmapRenderPassBegin.renderArea.offset = { 0, 0 };
		PointShadowmapRenderPassBegin.framebuffer = PointShadowmaps[pointLightIndex].framebuffer;

		VkClearValue clearValue{};
		clearValue.depthStencil = { 1.0f, 0 };
		PointShadowmapRenderPassBegin.clearValueCount = 1;
		PointShadowmapRenderPassBegin.pClearValues = &clearValue;

		vkCmdBeginRenderPass(DrawCommandBuffer, &PointShadowmapRenderPassBegin, VK_SUBPASS_CONTENTS_INLINE);


		vkCmdBindPipeline(DrawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PointShadowmapGraphicsPipeline);
		uint32_t dynamicOffset = 0;
		vkCmdBindDescriptorSets(DrawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PointShadowmapGraphicsPipelineLayout, 0, 1, &PointShadowmapDescriptorSet, 0, &dynamicOffset);

		drawIndexedPointShadow(0, pointLightIndex);

		for (size_t i = 2; i < Objects.size(); i++) {
			drawIndexedPointShadow(i, pointLightIndex);
		}


		vkCmdEndRenderPass(DrawCommandBuffer);
	}



	// render 3D scene
	VkRenderPassBeginInfo renderPassBegin{};
	renderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBegin.renderPass = RenderPass;
	renderPassBegin.renderArea.extent = SwapchainExtent;
	renderPassBegin.renderArea.offset = {0, 0};
	renderPassBegin.framebuffer = SwapchainFramebuffers[imageIndex];

	array<VkClearValue, 3> clearValues{};
	clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clearValues[1].depthStencil = { 1.0f, 0 };
	clearValues[2].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
	renderPassBegin.clearValueCount = 3;
	renderPassBegin.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(DrawCommandBuffer, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);


	// dynamic viewport settings
	VkViewport viewport;
	viewport.width = static_cast<float>(SwapchainExtent.width);
	viewport.height = -static_cast<float>(SwapchainExtent.height);
	viewport.x = 0.0f;
	viewport.y = static_cast<float>(SwapchainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor;
	scissor.extent = SwapchainExtent;
	scissor.offset = { 0, 0 };

	vkCmdSetViewport(DrawCommandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(DrawCommandBuffer, 0, 1, &scissor);


	uint32_t dynamicOffset = 0;
	vkCmdBindDescriptorSets(DrawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipelineLayout, 0, static_cast<uint32_t>(DescriptorSets.size()), DescriptorSets.data(), 0, &dynamicOffset);

	vkCmdBindPipeline(DrawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, FloorGraphicsPipeline);
	drawIndexed(0);

	vkCmdBindPipeline(DrawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, TextureGraphicsPipeline);
	for (size_t i = 2; i < Objects.size(); i++) {
		drawIndexed(i);
	}


	// light models
	vkCmdBindPipeline(DrawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, LightGraphicsPipeline);
	for (size_t i = 0; i < DirLightObjects.size(); i++) {
		drawIndexedDirLight(i);
	}

	for (size_t i = 0; i < SpotLightObjects.size(); i++) {
		drawIndexedSpotLight(i);
	}

	for (size_t i = 0; i < PointLightObjects.size(); i++) {
		drawIndexedPointLight(i);
	}


	// skybox
	vkCmdBindPipeline(DrawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, SkyboxGraphicsPipeline);	// draw skybox last to prevent overdraw
	drawIndexed(1);


	vkCmdEndRenderPass(DrawCommandBuffer);

	if (vkEndCommandBuffer(DrawCommandBuffer) != VK_SUCCESS) {
		cleanup();
		throw runtime_error("failed to end recording draw command to graphics command buffer!");
	}
}

void drawIndexedPointLight(size_t pointLightObjectIndex) {
	if (pointLightObjectIndex >= PointLightObjects.size()) {
		cleanup();
		throw runtime_error("Point Light Object Index is out of range!");
	}
	if (!PointLightObjects[pointLightObjectIndex].render) {
		return;
	}
	uint32_t modelIndex = PointLightObjects[pointLightObjectIndex].modelIndex;

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(DrawCommandBuffer, 0, 1, &LightModels[modelIndex].vertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(DrawCommandBuffer, LightModels[modelIndex].indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	XMMATRIX w = PointLightObjects[pointLightObjectIndex].W;
	uint32_t lightType = 2;
	uint32_t lightIndex = static_cast<uint32_t>(pointLightObjectIndex);

	XMMATRIX wvp = XMMatrixMultiply(w, VP);
	vkCmdPushConstants(DrawCommandBuffer, LightGraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(XMMATRIX), &wvp);
	vkCmdPushConstants(DrawCommandBuffer, LightGraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 64, sizeof(uint32_t), &lightType);
	vkCmdPushConstants(DrawCommandBuffer, LightGraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 68, sizeof(uint32_t), &lightIndex);

	vkCmdDrawIndexed(DrawCommandBuffer, LightModels[modelIndex].indexCount, 1, 0, 0, 0);
}

void drawIndexedSpotLight(size_t spotLightObjectIndex) {
	if (spotLightObjectIndex >= SpotLightObjects.size()) {
		cleanup();
		throw runtime_error("Spot Light Object Index is out of range!");
	}
	if (!SpotLightObjects[spotLightObjectIndex].render) {
		return;
	}
	uint32_t modelIndex = SpotLightObjects[spotLightObjectIndex].modelIndex;

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(DrawCommandBuffer, 0, 1, &LightModels[modelIndex].vertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(DrawCommandBuffer, LightModels[modelIndex].indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	XMMATRIX w = SpotLightObjects[spotLightObjectIndex].W;
	uint32_t lightType = 1;
	uint32_t lightIndex = static_cast<uint32_t>(spotLightObjectIndex);

	XMMATRIX wvp = XMMatrixMultiply(w, VP);
	vkCmdPushConstants(DrawCommandBuffer, LightGraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(XMMATRIX), &wvp);
	vkCmdPushConstants(DrawCommandBuffer, LightGraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 64, sizeof(uint32_t), &lightType);
	vkCmdPushConstants(DrawCommandBuffer, LightGraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 68, sizeof(uint32_t), &lightIndex);

	vkCmdDrawIndexed(DrawCommandBuffer, LightModels[modelIndex].indexCount, 1, 0, 0, 0);
}

void drawIndexedDirLight(size_t dirLightObjectIndex) {
	if (dirLightObjectIndex >= DirLightObjects.size()) {
		cleanup();
		throw runtime_error("Dir Light Object Index is out of range!");
	}
	if (!DirLightObjects[dirLightObjectIndex].render) {
		return;
	}
	uint32_t modelIndex = DirLightObjects[dirLightObjectIndex].modelIndex;

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(DrawCommandBuffer, 0, 1, &LightModels[modelIndex].vertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(DrawCommandBuffer, LightModels[modelIndex].indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	XMMATRIX w = DirLightObjects[dirLightObjectIndex].W;
	uint32_t lightType = 0;
	uint32_t lightIndex = static_cast<uint32_t>(dirLightObjectIndex);

	XMMATRIX wvp = XMMatrixMultiply(w, VP);
	vkCmdPushConstants(DrawCommandBuffer, LightGraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(XMMATRIX), &wvp);
	vkCmdPushConstants(DrawCommandBuffer, LightGraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 64, sizeof(uint32_t), &lightType);
	vkCmdPushConstants(DrawCommandBuffer, LightGraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 68, sizeof(uint32_t), &lightIndex);

	vkCmdDrawIndexed(DrawCommandBuffer, LightModels[modelIndex].indexCount, 1, 0, 0, 0);
}

void drawIndexed(size_t objectIndex) {
	if (objectIndex >= Objects.size()) {
		cleanup();
		throw runtime_error("Object Index is out of range!");
	}
	if (!Objects[objectIndex].render) {
		return;
	}
	uint32_t modelIndex = Objects[objectIndex].modelIndex;

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(DrawCommandBuffer, 0, 1, &Models[modelIndex].vertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(DrawCommandBuffer, Models[modelIndex].indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	XMMATRIX w = Objects[objectIndex].W;
	XMMATRIX normal_w = Objects[objectIndex].normalW;
	uint32_t textureIndex = Objects[objectIndex].textureIndex;

	XMMATRIX wvp = XMMatrixMultiply(w, VP);
	vkCmdPushConstants(DrawCommandBuffer, GraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(XMMATRIX), &wvp);
	vkCmdPushConstants(DrawCommandBuffer, GraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 64, sizeof(XMMATRIX), &w);
	vkCmdPushConstants(DrawCommandBuffer, GraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 128, sizeof(XMMATRIX), &normal_w);
	vkCmdPushConstants(DrawCommandBuffer, GraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 192, sizeof(uint32_t), &textureIndex);

	vkCmdDrawIndexed(DrawCommandBuffer, Models[modelIndex].indexCount, 1, 0, 0, 0);
}

void drawIndexedPointShadow(size_t objectIndex, size_t pointLightIndex) {
	if (objectIndex >= Objects.size()) {
		cleanup();
		throw runtime_error("Object Index is out of range!");
	}
	if (!Objects[objectIndex].render) {
		return;
	}
	uint32_t modelIndex = Objects[objectIndex].modelIndex;

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(DrawCommandBuffer, 0, 1, &Models[modelIndex].vertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(DrawCommandBuffer, Models[modelIndex].indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	XMMATRIX w = Objects[objectIndex].W;
	uint32_t pointLightIndex_uint32 = static_cast<uint32_t>(pointLightIndex);

	vkCmdPushConstants(DrawCommandBuffer, PointShadowmapGraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(XMMATRIX), &w);
	vkCmdPushConstants(DrawCommandBuffer, PointShadowmapGraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 64, sizeof(uint32_t), &pointLightIndex_uint32);

	vkCmdDrawIndexed(DrawCommandBuffer, Models[modelIndex].indexCount, 1, 0, 0, 0);
}

void drawIndexedSpotShadow(size_t objectIndex, size_t spotLightIndex) {
	if (objectIndex >= Objects.size()) {
		cleanup();
		throw runtime_error("Object Index is out of range!");
	}
	if (!Objects[objectIndex].render) {
		return;
	}
	uint32_t modelIndex = Objects[objectIndex].modelIndex;

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(DrawCommandBuffer, 0, 1, &Models[modelIndex].vertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(DrawCommandBuffer, Models[modelIndex].indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	XMMATRIX w = Objects[objectIndex].W;

	XMMATRIX light_wvp = XMMatrixMultiply(w, SpotLightVPMatrices[spotLightIndex]);
	vkCmdPushConstants(DrawCommandBuffer, DirSpotShadowmapGraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(XMMATRIX), &light_wvp);

	vkCmdDrawIndexed(DrawCommandBuffer, Models[modelIndex].indexCount, 1, 0, 0, 0);
}

void drawIndexedDirShadow(size_t objectIndex, size_t dirLightIndex) {
	if (objectIndex >= Objects.size()) {
		cleanup();
		throw runtime_error("Object Index is out of range!");
	}
	if (!Objects[objectIndex].render) {
		return;
	}
	uint32_t modelIndex = Objects[objectIndex].modelIndex;

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(DrawCommandBuffer, 0, 1, &Models[modelIndex].vertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(DrawCommandBuffer, Models[modelIndex].indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	XMMATRIX w = Objects[objectIndex].W;

	XMMATRIX light_wvp = XMMatrixMultiply(w, DirLightVPMatrices[dirLightIndex]);
	vkCmdPushConstants(DrawCommandBuffer, DirSpotShadowmapGraphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(XMMATRIX), &light_wvp);

	vkCmdDrawIndexed(DrawCommandBuffer, Models[modelIndex].indexCount, 1, 0, 0, 0);
}


void recreateSwapchain() {
	// framebuffers
	for (size_t i = 0; i < SwapchainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(Device, SwapchainFramebuffers[i], nullptr);
		SwapchainFramebuffers[i] = VK_NULL_HANDLE;
	}

	// render pass
	if (RenderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(Device, RenderPass, nullptr);
		RenderPass = VK_NULL_HANDLE;
	}


	// color & depth resources
	if (DepthImageView != VK_NULL_HANDLE) {
		vkDestroyImageView(Device, DepthImageView, nullptr);
		DepthImageView = VK_NULL_HANDLE;
	}

	if (DepthImageMemory != VK_NULL_HANDLE) {
		vkFreeMemory(Device, DepthImageMemory, nullptr);
		DepthImageMemory = VK_NULL_HANDLE;
	}

	if (DepthImage != VK_NULL_HANDLE) {
		vkDestroyImage(Device, DepthImage, nullptr);
		DepthImage = VK_NULL_HANDLE;
	}

	if (ColorImageView != VK_NULL_HANDLE) {
		vkDestroyImageView(Device, ColorImageView, nullptr);
		ColorImageView = VK_NULL_HANDLE;
	}

	if (ColorImageMemory != VK_NULL_HANDLE) {
		vkFreeMemory(Device, ColorImageMemory, nullptr);
		ColorImageMemory = VK_NULL_HANDLE;
	}

	if (ColorImage != VK_NULL_HANDLE) {
		vkDestroyImage(Device, ColorImage, nullptr);
		ColorImage = VK_NULL_HANDLE;
	}


	// swap chain
	for (size_t i = 0; i < SwapchainImageViews.size(); i++) {
		vkDestroyImageView(Device, SwapchainImageViews[i], nullptr);
		SwapchainImageViews[i] = VK_NULL_HANDLE;
	}

	if (Swapchain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(Device, Swapchain, nullptr);
		Swapchain = VK_NULL_HANDLE;
	}



	// if window size is set to 0, wait until it is set back to a valid size
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(Window, &width, &height);
	while(width == 0 || height == 0){
		glfwGetFramebufferSize(Window, &width, &height);
		glfwWaitEvents();
	}


	bool result;

	result = createSwapchain(&SwapchainFormat, &SwapchainExtent, PhysicalDevice, Surface, Window, QueueFamilies, Device, &Swapchain, &SwapchainImages);
	processResult(result, "createSwapchain() failed!");

	result = createSwapchainImageViews(SwapchainFormat, Device, &SwapchainImages, &SwapchainImageViews);
	processResult(result, "createSwapchainImageViews() failed!");

	result = createColorResources(SwapchainFormat, SwapchainExtent, MSAA_SAMPLE_COUNT, GraphicsCommandPool, GraphicsQueue, PhysicalDevice, Device, &ColorImage, &ColorImageMemory, &ColorImageView);
	processResult(result, "createColorResources() failed!");

	result = createDepthResources(&DepthImageFormat, SwapchainExtent, MSAA_SAMPLE_COUNT, GraphicsCommandPool, GraphicsQueue, PhysicalDevice, Device, &DepthImage, &DepthImageMemory, &DepthImageView);
	processResult(result, "createDepthResources() failed!");

	result = createRenderPass(SwapchainFormat, DepthImageFormat, MSAA_SAMPLE_COUNT, Device, &RenderPass);
	processResult(result, "createRenderPass() failed!");

	result = createFramebuffers(RenderPass, SwapchainExtent, Device, ColorImageView, DepthImageView, &SwapchainImageViews, &SwapchainFramebuffers);
	processResult(result, "createFramebuffers() failed!");
}



void update() {
	auto currentTime = chrono::high_resolution_clock::now();
	float frameTime = chrono::duration<float, chrono::seconds::period>(currentTime - LastTime).count();
	LastTime = currentTime;
	//cout << "Frametime: " << frameTime << "\tFPS: " << 1.0f/frameTime << endl;


	updateCamera(frameTime);
	XMMATRIX v = getViewMatrixCamera(CameraPosition, CameraRotation);
	XMMATRIX p = XMMatrixPerspectiveFovLH(90.0f * Deg2Rad, (static_cast<float>(SwapchainExtent.width)/static_cast<float>(SwapchainExtent.height)), 0.1f, 1000.0f);
	VP = XMMatrixMultiply(v, p);


	updateLights(frameTime);


	//RotationAngle += 45.0f * frameTime;
	//if (RotationAngle >= 360.0f) {
	//	RotationAngle -= 360.0f;
	//}
	//
	//Transform transform;
	//XMMATRIX RS_M;
	//
	//for (Object& object : Objects){
	//	if (object.update) {
	//		//something I don't know				
	//
	//		object.transform.rotation.y = Deg2Rad * RotationAngle;
	//		transform = object.transform;
	//		RS_M = RS(transform);
	//
	//		object.W = TRS(transform, RS_M);
	//		object.normalW = XMMatrixTranspose(XMMatrixInverse(nullptr, RS_M));
	//	}
	//}
}

void updateLights(float frameTime) {
	// Directional Lights
	XMMATRIX ortho_P = XMMatrixOrthographicLH(20.0f, 20.0f, 0.1f, 1000.0f);
	for (size_t i = 0; i < DirLights.size(); i++) {
		// update light
		//DirLights[i];


		// update vp matrices
		XMMATRIX light_V = getViewMatrixLight(DirLights[i].pos, DirLights[i].dir);
		DirLightVPMatrices[i] = XMMatrixMultiply(light_V, ortho_P);


		// update model
		Light light = DirLights[i];

		DirLightObjects[i].transform.translation = light.pos;
		DirLightObjects[i].transform.rotation = XMFLOAT3(Deg2Rad * light.rot.x, Deg2Rad * light.rot.y, 0.0f);
		Transform transform = DirLightObjects[i].transform;

		DirLightObjects[i].W = TRS(transform, RS(transform));
	}

	// update buffers
	if (!DirLights.empty()) {
		void* data;
	
		// Light Data
		vkMapMemory(Device, DirLightsBuffer.bufferMemory, 0, DirLightsBuffer.bufferSize, 0, &data);
		memcpy(data, (void*) DirLights.data(), DirLightsBuffer.bufferSize);
		vkUnmapMemory(Device, DirLightsBuffer.bufferMemory);
	
		// Light Matrix Data
		vkMapMemory(Device, DirLightVPMatricesBuffer.bufferMemory, 0, DirLightVPMatricesBuffer.bufferSize, 0, &data);
		memcpy(data, (void*) DirLightVPMatrices.data(), DirLightVPMatricesBuffer.bufferSize);
		vkUnmapMemory(Device, DirLightVPMatricesBuffer.bufferMemory);
	}



	// Spot Lights
	XMMATRIX pers_P = XMMatrixPerspectiveFovLH(90.0f * Deg2Rad, (static_cast<float>(SHADOWMAP_EXTENT.width) / static_cast<float>(SHADOWMAP_EXTENT.height)), 0.1f, 1000.0f);
	for (size_t i = 0; i < SpotLights.size(); i++) {
		// update light
		//SpotLights[i];


		// update vp matrices
		XMMATRIX light_V = getViewMatrixLight(SpotLights[i].pos, SpotLights[i].dir);
		SpotLightVPMatrices[i] = XMMatrixMultiply(light_V, pers_P);


		// update model
		Light light = SpotLights[i];

		SpotLightObjects[i].transform.translation = light.pos;
		SpotLightObjects[i].transform.rotation = XMFLOAT3(Deg2Rad * light.rot.x, Deg2Rad * light.rot.y, 0.0f);
		Transform transform = SpotLightObjects[i].transform;

		SpotLightObjects[i].W = TRS(transform, RS(transform));
	}

	// update buffers
	if (!SpotLights.empty()) {
		void* data;
	
		// Light Data
		vkMapMemory(Device, SpotLightsBuffer.bufferMemory, 0, SpotLightsBuffer.bufferSize, 0, &data);
		memcpy(data, (void*) SpotLights.data(), SpotLightsBuffer.bufferSize);
		vkUnmapMemory(Device, SpotLightsBuffer.bufferMemory);
	
		// Light Matrix Data
		vkMapMemory(Device, SpotLightVPMatricesBuffer.bufferMemory, 0, SpotLightVPMatricesBuffer.bufferSize, 0, &data);
		memcpy(data, (void*) SpotLightVPMatrices.data(), SpotLightVPMatricesBuffer.bufferSize);
		vkUnmapMemory(Device, SpotLightVPMatricesBuffer.bufferMemory);
	}



	// Point Lights
	//RotationAngle += 45.0f * frameTime;
	//if (RotationAngle >= 360.0f) {
	//	RotationAngle -= 360.0f;
	//}

	for (size_t i = 0; i < PointLights.size(); i++) {
		// update light
		//PointLights[i].pos.z = 2.0f * cosf(Deg2Rad * RotationAngle);
		//PointLights[i].pos.x = 2.0f * sinf(Deg2Rad * RotationAngle) + 0.5f;


		// update vp matrices
		PointLightVPMatrices[i*6 + 0] = XMMatrixMultiply(getViewMatrixPointLight(PointLights[i].pos, {-1.0f,  0.0f,  0.0f}, {0.0f, -1.0f,  0.0f}), pers_P);
		PointLightVPMatrices[i*6 + 1] = XMMatrixMultiply(getViewMatrixPointLight(PointLights[i].pos, { 1.0f,  0.0f,  0.0f}, {0.0f, -1.0f,  0.0f}), pers_P);
		PointLightVPMatrices[i*6 + 2] = XMMatrixMultiply(getViewMatrixPointLight(PointLights[i].pos, { 0.0f,  1.0f,  0.0f}, {0.0f,  0.0f,  1.0f}), pers_P);
		PointLightVPMatrices[i*6 + 3] = XMMatrixMultiply(getViewMatrixPointLight(PointLights[i].pos, { 0.0f, -1.0f,  0.0f}, {0.0f,  0.0f, -1.0f}), pers_P);
		PointLightVPMatrices[i*6 + 4] = XMMatrixMultiply(getViewMatrixPointLight(PointLights[i].pos, { 0.0f,  0.0f,  1.0f}, {0.0f, -1.0f,  0.0f}), pers_P);
		PointLightVPMatrices[i*6 + 5] = XMMatrixMultiply(getViewMatrixPointLight(PointLights[i].pos, { 0.0f,  0.0f, -1.0f}, {0.0f, -1.0f,  0.0f}), pers_P);


		// update model
		Light light = PointLights[i];

		PointLightObjects[i].transform.translation = light.pos;
		PointLightObjects[i].transform.rotation = XMFLOAT3(Deg2Rad * light.rot.x, Deg2Rad * light.rot.y, 0.0f);
		Transform transform = PointLightObjects[i].transform;

		PointLightObjects[i].W = TRS(transform, RS(transform));
	}

	// update buffers
	if (!PointLights.empty()) {
		void* data;
	
		// Light Data
		vkMapMemory(Device, PointLightsBuffer.bufferMemory, 0, PointLightsBuffer.bufferSize, 0, &data);
		memcpy(data, (void*) PointLights.data(), PointLightsBuffer.bufferSize);
		vkUnmapMemory(Device, PointLightsBuffer.bufferMemory);
	
		// Light Matrix Data
		vkMapMemory(Device, PointLightVPMatricesBuffer.bufferMemory, 0, PointLightVPMatricesBuffer.bufferSize, 0, &data);
		memcpy(data, (void*) PointLightVPMatrices.data(), PointLightVPMatricesBuffer.bufferSize);
		vkUnmapMemory(Device, PointLightVPMatricesBuffer.bufferMemory);
	}
}

XMMATRIX getViewMatrixCamera(XMFLOAT3 cameraPosition, XMFLOAT3 cameraRotation) {
	XMVECTOR eyePosition = XMLoadFloat3(&cameraPosition);
	
	XMFLOAT3 focusDirectionF3 = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMVECTOR focusDirection = XMLoadFloat3(&focusDirectionF3);

	XMFLOAT3 upDirectionF3 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMVECTOR upDirection = XMLoadFloat3(&upDirectionF3);


	// Rotate focusDirection and UpDirection according to camera rotation
	XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(cameraRotation.x * Deg2Rad, cameraRotation.y * Deg2Rad, cameraRotation.z * Deg2Rad);
	focusDirection = XMVector3TransformCoord(focusDirection, rotMat);


	XMVECTOR focusPosition = XMVectorAdd(focusDirection, eyePosition);	// focusDirection -> focusPosition
	return XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
}

void updateCamera(float frameTime) {
	double xpos, ypos;
	glfwGetCursorPos(Window, &xpos, &ypos);

	DeltaCursorXPos = static_cast<float>(-LastCursorXPos + xpos);
	DeltaCursorYPos = static_cast<float>(-LastCursorYPos + ypos);

	LastCursorXPos = xpos;
	LastCursorYPos = ypos;



	// CAMERA CONTROL PARAMETERS
	float mouseSensitivity = 10.0f;
	float walkSpeed = 4.0f;
	float runSpeed = 8.0f;
	float flySpeed = 6.0f;


	// ROTATION
	CameraRotation.y += DeltaCursorXPos * mouseSensitivity * frameTime;
	CameraRotation.x += DeltaCursorYPos * mouseSensitivity * frameTime;
	
	// Keep Y Rotation in range [0, 360]
	if (CameraRotation.y < 0.0f)
		CameraRotation.y += 360.f;
	
	else if (CameraRotation.y > 360.0f)
		CameraRotation.y -= 360.f;
	
	
	// Keep X Rotation in range [-80, 90]
	if (CameraRotation.x < -80.0f)
		CameraRotation.x = -80.0f;
	
	else if (CameraRotation.x > 89.0f)
		CameraRotation.x = 89.0f;
	
	

	// Note, flying and walking/running at the same time will lead to a higher max speed sqrt(flySpeed^2 + walkSpeed^2);
	//Y POSITION
	if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
		CameraPosition.y = CameraPosition.y + flySpeed * frameTime;

	if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
		CameraPosition.y = CameraPosition.y - flySpeed * frameTime;



	// X-Z POSITION
	float movementSpeed = walkSpeed;
	if (glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		movementSpeed = runSpeed;
	
	
	XMFLOAT2 moveInputDir = XMFLOAT2(0.0f, 0.0f);
	
	// Get Inputs
	if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
		moveInputDir.y = moveInputDir.y + 1.0f;
	
	if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
		moveInputDir.y = moveInputDir.y - 1.0f;
	
	if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
		moveInputDir.x = moveInputDir.x + 1.0f;
	
	if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
		moveInputDir.x = moveInputDir.x - 1.0f;
	
	
	// Normalize Input Vector
	float mag = sqrtf(moveInputDir.x * moveInputDir.x  +  moveInputDir.y * moveInputDir.y);
	if (mag < 0.000001f)
		return;
	moveInputDir.x = moveInputDir.x / mag;
	moveInputDir.y = moveInputDir.y / mag;
	
	
	// Rotate according to camera direction so movement is relative to the camera
	float theta = -CameraRotation.y * Deg2Rad;
	XMFLOAT2 velocity;
	velocity.x = moveInputDir.x * cosf(theta) - moveInputDir.y * sinf(theta);
	velocity.y = moveInputDir.x * sinf(theta) + moveInputDir.y * cosf(theta);
	
	// Set velocity
	velocity = XMFLOAT2(velocity.x * movementSpeed, velocity.y * movementSpeed);
	
	// Move
	CameraPosition.x = CameraPosition.x + velocity.x * frameTime;
	CameraPosition.z = CameraPosition.z + velocity.y * frameTime;
}



void init() {
	bool result;


	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	//Window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan", nullptr, nullptr);
	//Window = glfwCreateWindow(1920, 1080, "Vulkan", nullptr, nullptr);
	Window = glfwCreateWindow(1920, 1080, "Vulkan", glfwGetPrimaryMonitor(), nullptr);


	result = createInstance(ENABLE_VALIDATION_LAYERS, &VALIDATION_LAYERS, &DEVICE_EXTENSIONS, &Instance);
	processResult(result, "createInstance() failed!");
	
	if (ENABLE_VALIDATION_LAYERS) {
		result = setupDebugMessenger(Instance, &DebugMessenger);
		processResult(result, "setupDebugMessenger() failed!");
	}

	result = createSurface(Instance, Window, &Surface);
	processResult(result, "createSurface() failed!");

	result = pickPhysicalDevice(Instance, Surface, &PhysicalDevice, &QueueFamilies, &DEVICE_EXTENSIONS);
	processResult(result, "pickPhysicalDevice() failed!");

	result = createLogicalDevice(&DEVICE_EXTENSIONS, QueueFamilies,
								 ENABLE_VALIDATION_LAYERS, &VALIDATION_LAYERS,
								 PhysicalDevice, &Device, &GraphicsQueue, &PresentQueue);
	processResult(result, "createLogicalDevice() failed!");

	result = createGraphicsCommandPool(QueueFamilies, Device, &GraphicsCommandPool);
	processResult(result, "createGraphicsCommandPool() failed!");



	result = createSwapchain(&SwapchainFormat, &SwapchainExtent, PhysicalDevice, Surface, Window, QueueFamilies, Device, &Swapchain, &SwapchainImages);
	processResult(result, "createSwapchain() failed!");
	
	result = createSwapchainImageViews(SwapchainFormat, Device, &SwapchainImages, &SwapchainImageViews);
	processResult(result, "createSwapchainImageViews() failed!");

	result = createColorResources(SwapchainFormat, SwapchainExtent, MSAA_SAMPLE_COUNT, GraphicsCommandPool, GraphicsQueue, PhysicalDevice, Device, &ColorImage, &ColorImageMemory, &ColorImageView);
	processResult(result, "createColorResources() failed!");

	result = createDepthResources(&DepthImageFormat, SwapchainExtent, MSAA_SAMPLE_COUNT, GraphicsCommandPool, GraphicsQueue, PhysicalDevice, Device, &DepthImage, &DepthImageMemory, &DepthImageView);
	processResult(result, "createDepthResources() failed!");

	result = createRenderPass(SwapchainFormat, DepthImageFormat, MSAA_SAMPLE_COUNT, Device, &RenderPass);
	processResult(result, "createRenderPass() failed!");

	result = createFramebuffers(RenderPass, SwapchainExtent, Device, ColorImageView, DepthImageView, &SwapchainImageViews, &SwapchainFramebuffers);
	processResult(result, "createFramebuffers() failed!");



	result = createTextures(GraphicsQueue, GraphicsCommandPool, PhysicalDevice, Device, &SkyboxTexture, &Albedomaps);
	processResult(result, "createTextures() failed!");

	result = createModelsAndObjects(GraphicsQueue, GraphicsCommandPool, PhysicalDevice, Device, &Albedomaps, &Models, &Objects);
	processResult(result, "createModelsAndObjects() failed!");



	result = createLightBuffers(SHADOWMAP_EXTENT, PhysicalDevice, Device, &NumLightsBuffer,
								MAX_NUM_DIR_LIGHTS, &DirLights, &DirLightsBuffer, &DirLightVPMatrices, &DirLightVPMatricesBuffer,
								MAX_NUM_SPOT_LIGHTS, &SpotLights, &SpotLightsBuffer, &SpotLightVPMatrices, &SpotLightVPMatricesBuffer,
								MAX_NUM_POINT_LIGHTS, &PointLights, &PointLightsBuffer, &PointLightVPMatrices, &PointLightVPMatricesBuffer);
	processResult(result, "createLightBuffers() failed!");

	result = createLightModelsAndObjects(GraphicsQueue, GraphicsCommandPool, PhysicalDevice, Device,
										 &DirLights, &SpotLights, &PointLights,
										 &LightModels, &DirLightObjects, &SpotLightObjects, &PointLightObjects);
	processResult(result, "createLightModelsAndObjects() failed!");



	result = createShadowmapResources(GraphicsCommandPool, GraphicsQueue, PhysicalDevice, Device,
									  &ShadowmapSampler, &DirSpotShadowmapRenderPass, &PointShadowmapRenderPass,
									  SHADOWMAP_EXTENT, static_cast<uint32_t>(DirLights.size()), &DirShadowmaps, static_cast<uint32_t>(SpotLights.size()), &SpotShadowmaps,
									  static_cast<uint32_t>(PointLights.size()), &PointShadowmaps);
	processResult(result, "createShadowmapResources() failed!");

	result = createPointShadowmapDescriptorResources(NumLightsBuffer, PointLightVPMatricesBuffer, PointLightsBuffer,
													 Device, &PointShadowmapDescriptorSetLayout, &PointShadowmapDescriptorPool, &PointShadowmapDescriptorSet);
	processResult(result, "createPointShadowmapDescriptorResources() failed!");

	result = createShadowmapGraphicsPipelines(SHADOWMAP_EXTENT, DirSpotShadowmapRenderPass, PointShadowmapRenderPass, Device,
											  &DirSpotShadowmapGraphicsPipelineLayout, &DirSpotShadowmapGraphicsPipeline,
											  &PointShadowmapDescriptorSetLayout, &PointShadowmapGraphicsPipelineLayout, &PointShadowmapGraphicsPipeline);
	processResult(result, "createShadowmapGraphicsPipeline() failed!");



	result = createSampler(Device, &Sampler);
	processResult(result, "createSampler() failed!");

	result = createDescriptorResources(Sampler, SkyboxTexture.imageView, MAX_NUM_ALBEDOMAPS, static_cast<uint32_t>(Albedomaps.size()), &Albedomaps,
									   MAX_NUM_DIR_LIGHTS, MAX_NUM_SPOT_LIGHTS, MAX_NUM_POINT_LIGHTS, NumLightsBuffer, ShadowmapSampler,
									   DirLightsBuffer, DirLightVPMatricesBuffer, static_cast<uint32_t>(DirLights.size()), &DirShadowmaps,
									   SpotLightsBuffer, SpotLightVPMatricesBuffer, static_cast<uint32_t>(SpotLights.size()), &SpotShadowmaps,
									   PointLightsBuffer, PointLightVPMatricesBuffer, static_cast<uint32_t>(PointLights.size()), &PointShadowmaps,
									   Device, &DescriptorSetLayouts, &DescriptorPool, &DescriptorSets);
	processResult(result, "createDescriptorResources() failed!");

	result = createGraphicsPipelines(SwapchainExtent, MSAA_SAMPLE_COUNT, RenderPass, &DescriptorSetLayouts, Device,
									 &GraphicsPipelineLayout, &LightGraphicsPipelineLayout,
									 &FloorGraphicsPipeline, &SkyboxGraphicsPipeline, &TextureGraphicsPipeline, &LightGraphicsPipeline);
	processResult(result, "createGraphicsPipelines() failed!");


	result = createSyncObjects(Device, &ImageAcquiredSemaphore, &RenderCompleteSemaphore, &FrameFence);
	processResult(result, "createSyncObjects() failed!");

	result = createDrawCommandBuffer(GraphicsCommandPool, Device, &DrawCommandBuffer);
	processResult(result, "createDrawCommandBuffer() failed!");



	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetInputMode(Window, GLFW_STICKY_KEYS, 1);
	glfwSetInputMode(Window, GLFW_STICKY_MOUSE_BUTTONS, 1);

	glfwSetKeyCallback(Window, keyCallback);
	//glfwSetCursorPosCallback(Window, cursorPosCallback);
	glfwSetWindowSizeCallback(Window, windowSizeCallback);
}

void windowSizeCallback(GLFWwindow* window, int width, int height) {
	cout << "Window Size - " << width << " : " << height << endl;
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	cout << "Cursor Position - " << xPos << " : " << yPos << endl;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		cout << "Escape key is pressed" << endl;
	}
}


bool createDrawCommandBuffer(VkCommandPool graphicsCommandPool, VkDevice device, VkCommandBuffer* pDrawCommandBuffer) {
	if (pDrawCommandBuffer == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	allocInfo.commandPool = graphicsCommandPool;

	if (vkAllocateCommandBuffers(device, &allocInfo, pDrawCommandBuffer) != VK_SUCCESS) {
		cerr << "failed to allocate draw command buffer!" << endl;
		return false;
	}

	return true;
}

bool createSyncObjects(VkDevice device, VkSemaphore* pImageAcquiredSemaphore, VkSemaphore* pRenderCompleteSemaphore, VkFence* pFrameFence) {
	if (pImageAcquiredSemaphore == nullptr || pRenderCompleteSemaphore == nullptr || pFrameFence == nullptr) {
		cerr << NULLPTR_MESSAGE << endl;
		return false;
	}

	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, pImageAcquiredSemaphore) ||
		vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, pRenderCompleteSemaphore) ||
		vkCreateFence(device, &fenceCreateInfo, nullptr, pFrameFence)
		!= VK_SUCCESS) {
		cerr << "failed to create sync objects!" << endl;
		return false;
	}

	return true;
}


void processResult(bool result, const char* errorMessage) {
	if (!result) {
		cleanup();
		throw runtime_error(errorMessage);
	}
}


void cleanup() {
	// draw command buffer
	if (DrawCommandBuffer != VK_NULL_HANDLE) {
		vkFreeCommandBuffers(Device, GraphicsCommandPool, 1, &DrawCommandBuffer);
		DrawCommandBuffer = VK_NULL_HANDLE;
	}

	// sync objects
	if (FrameFence != VK_NULL_HANDLE) {
		vkDestroyFence(Device, FrameFence, nullptr);
		FrameFence = VK_NULL_HANDLE;
	}

	if (RenderCompleteSemaphore != VK_NULL_HANDLE) {
		vkDestroySemaphore(Device, RenderCompleteSemaphore, nullptr);
		RenderCompleteSemaphore = VK_NULL_HANDLE;
	}

	if (ImageAcquiredSemaphore != VK_NULL_HANDLE) {
		vkDestroySemaphore(Device, ImageAcquiredSemaphore, nullptr);
		ImageAcquiredSemaphore = VK_NULL_HANDLE;
	}


	// graphics pipelines
	if (LightGraphicsPipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(Device, LightGraphicsPipeline, nullptr);
		LightGraphicsPipeline = VK_NULL_HANDLE;
	}

	if (TextureGraphicsPipeline != VK_NULL_HANDLE) { 
		vkDestroyPipeline(Device, TextureGraphicsPipeline, nullptr);
		TextureGraphicsPipeline = VK_NULL_HANDLE;
	}

	if (SkyboxGraphicsPipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(Device, SkyboxGraphicsPipeline, nullptr);
		SkyboxGraphicsPipeline = VK_NULL_HANDLE;
	}

	if (FloorGraphicsPipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(Device, FloorGraphicsPipeline, nullptr);
		FloorGraphicsPipeline = VK_NULL_HANDLE;
	}

	if (LightGraphicsPipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(Device, LightGraphicsPipelineLayout, nullptr);
		LightGraphicsPipelineLayout = VK_NULL_HANDLE;
	}

	if (GraphicsPipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(Device, GraphicsPipelineLayout, nullptr);
		GraphicsPipelineLayout = VK_NULL_HANDLE;
	}

	
	// descriptor resources
	if (!DescriptorSets.empty()) {
		vkFreeDescriptorSets(Device, DescriptorPool, static_cast<uint32_t>(DescriptorSets.size()), DescriptorSets.data());
	}

	if (DescriptorPool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(Device, DescriptorPool, nullptr);
		DescriptorPool = VK_NULL_HANDLE;
	}

	for (auto& descriptorSetLayout : DescriptorSetLayouts){
		if (descriptorSetLayout != VK_NULL_HANDLE) {
			vkDestroyDescriptorSetLayout(Device, descriptorSetLayout, nullptr);
			descriptorSetLayout = VK_NULL_HANDLE;
		}
	}

	// sampler
	if (Sampler != VK_NULL_HANDLE) {
		vkDestroySampler(Device, Sampler, nullptr);
		Sampler = VK_NULL_HANDLE;
	}



	// shadows
	if (PointShadowmapGraphicsPipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(Device, PointShadowmapGraphicsPipeline, nullptr);
		PointShadowmapGraphicsPipeline = VK_NULL_HANDLE;
	}
	
	if (DirSpotShadowmapGraphicsPipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(Device, DirSpotShadowmapGraphicsPipeline, nullptr);
		DirSpotShadowmapGraphicsPipeline = VK_NULL_HANDLE;
	}


	if (PointShadowmapGraphicsPipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(Device, PointShadowmapGraphicsPipelineLayout, nullptr);
		PointShadowmapGraphicsPipelineLayout = VK_NULL_HANDLE;
	}

	if (DirSpotShadowmapGraphicsPipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(Device, DirSpotShadowmapGraphicsPipelineLayout, nullptr);
		DirSpotShadowmapGraphicsPipelineLayout = VK_NULL_HANDLE;
	}


	if (PointShadowmapDescriptorSet != VK_NULL_HANDLE) {
		vkFreeDescriptorSets(Device, PointShadowmapDescriptorPool, 1, &PointShadowmapDescriptorSet);
	}

	if (PointShadowmapDescriptorPool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(Device, PointShadowmapDescriptorPool, nullptr);
		PointShadowmapDescriptorPool = VK_NULL_HANDLE;
	}

	if (PointShadowmapDescriptorSetLayout != VK_NULL_HANDLE) {
		vkDestroyDescriptorSetLayout(Device, PointShadowmapDescriptorSetLayout, nullptr);
		PointShadowmapDescriptorSetLayout = VK_NULL_HANDLE;
	}


	for (size_t i = 0; i < PointShadowmaps.size(); i++) {
		cleanupImage(&PointShadowmaps[i]);
	}

	for (size_t i = 0; i < SpotShadowmaps.size(); i++) {
		cleanupImage(&SpotShadowmaps[i]);
	}

	for (size_t i = 0; i < DirShadowmaps.size(); i++){
		cleanupImage(&DirShadowmaps[i]);
	}


	if (PointShadowmapRenderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(Device, PointShadowmapRenderPass, nullptr);
		PointShadowmapRenderPass = VK_NULL_HANDLE;
	}

	if (DirSpotShadowmapRenderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(Device, DirSpotShadowmapRenderPass, nullptr);
		DirSpotShadowmapRenderPass = VK_NULL_HANDLE;
	}


	if (ShadowmapSampler != VK_NULL_HANDLE) {
		vkDestroySampler(Device, ShadowmapSampler, nullptr);
		ShadowmapSampler = VK_NULL_HANDLE;
	}



	// light model data
	for (Model& object : LightModels) {
		cleanupBuffer(&object.indexBuffer);
		cleanupBuffer(&object.vertexBuffer);
	}

	// light buffers
	cleanupBuffer(&PointLightVPMatricesBuffer);
	cleanupBuffer(&PointLightsBuffer);
	cleanupBuffer(&SpotLightVPMatricesBuffer);
	cleanupBuffer(&SpotLightsBuffer);
	cleanupBuffer(&DirLightVPMatricesBuffer);
	cleanupBuffer(&DirLightsBuffer);
	cleanupBuffer(&NumLightsBuffer);


	// model data
	for (Model& object : Models) {
		cleanupBuffer(&object.indexBuffer);
		cleanupBuffer(&object.vertexBuffer);
	}

	// textures
	for (size_t i = 0; i < Albedomaps.size(); i++) {
		cleanupTexture(&Albedomaps[i]);
	}
	cleanupTexture(&SkyboxTexture);

	

	// framebuffers
	for (size_t i = 0; i < SwapchainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(Device, SwapchainFramebuffers[i], nullptr);
		SwapchainFramebuffers[i] = VK_NULL_HANDLE;
	}

	// render pass
	if (RenderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(Device, RenderPass, nullptr);
		RenderPass = VK_NULL_HANDLE;
	}


	// color & depth resources
	if (DepthImageView != VK_NULL_HANDLE) {
		vkDestroyImageView(Device, DepthImageView, nullptr);
		DepthImageView = VK_NULL_HANDLE;
	}

	if (DepthImageMemory != VK_NULL_HANDLE) {
		vkFreeMemory(Device, DepthImageMemory, nullptr);
		DepthImageMemory = VK_NULL_HANDLE;
	}

	if (DepthImage != VK_NULL_HANDLE) {
		vkDestroyImage(Device, DepthImage, nullptr);
		DepthImage = VK_NULL_HANDLE;
	}

	if (ColorImageView != VK_NULL_HANDLE) {
		vkDestroyImageView(Device, ColorImageView, nullptr);
		ColorImageView = VK_NULL_HANDLE;
	}

	if (ColorImageMemory != VK_NULL_HANDLE) {
		vkFreeMemory(Device, ColorImageMemory, nullptr);
		ColorImageMemory = VK_NULL_HANDLE;
	}

	if (ColorImage != VK_NULL_HANDLE) {
		vkDestroyImage(Device, ColorImage, nullptr);
		ColorImage = VK_NULL_HANDLE;
	}


	// swap chain
	for (size_t i = 0; i < SwapchainImageViews.size(); i++) {
		vkDestroyImageView(Device, SwapchainImageViews[i], nullptr);
		SwapchainImageViews[i] = VK_NULL_HANDLE;
	}

	if (Swapchain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(Device, Swapchain, nullptr);
		Swapchain = VK_NULL_HANDLE;
	}


	// device and command pool
	if (GraphicsCommandPool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(Device, GraphicsCommandPool, nullptr);
		GraphicsCommandPool = VK_NULL_HANDLE;
	}

	if (Device != VK_NULL_HANDLE) {
		vkDestroyDevice(Device, nullptr);
		Device = VK_NULL_HANDLE;
	}


	// surface
	if (Surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(Instance, Surface, nullptr);
		Surface = VK_NULL_HANDLE;
	}


	// window and instance
	if (DebugMessenger != VK_NULL_HANDLE) {
		DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
		DebugMessenger = VK_NULL_HANDLE;
	}

	if (Instance != VK_NULL_HANDLE) {
		vkDestroyInstance(Instance, nullptr);
		Instance = VK_NULL_HANDLE;
	}

	if (Window) {
		glfwDestroyWindow(Window);
		Window = nullptr;
	}
	glfwTerminate();
}

void cleanupTexture(Texture* pTexture) {
	if (pTexture->imageView != VK_NULL_HANDLE) {
		vkDestroyImageView(Device, pTexture->imageView, nullptr);
		pTexture->imageView = VK_NULL_HANDLE;
	}

	if (pTexture->imageMemory != VK_NULL_HANDLE) {
		vkFreeMemory(Device, pTexture->imageMemory, nullptr);
		pTexture->imageMemory = VK_NULL_HANDLE;
	}

	if (pTexture->image != VK_NULL_HANDLE) {
		vkDestroyImage(Device, pTexture->image, nullptr);
		pTexture->image = VK_NULL_HANDLE;
	}
}

void cleanupBuffer(Buffer* pBuffer) {
	if (pBuffer->bufferMemory != VK_NULL_HANDLE) {
		vkFreeMemory(Device, pBuffer->bufferMemory, nullptr);
		pBuffer->bufferMemory = VK_NULL_HANDLE;
	}

	if (pBuffer->buffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(Device, pBuffer->buffer, nullptr);
		pBuffer->buffer = VK_NULL_HANDLE;
	}
}

void cleanupImage(Image* pImage) {
	if (pImage->framebuffer != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(Device, pImage->framebuffer, nullptr);
		pImage->framebuffer = VK_NULL_HANDLE;
	}

	if (pImage->imageView != VK_NULL_HANDLE) {
		vkDestroyImageView(Device, pImage->imageView, nullptr);
		pImage->imageView = VK_NULL_HANDLE;
	}

	if (pImage->imageMemory != VK_NULL_HANDLE) {
		vkFreeMemory(Device, pImage->imageMemory, nullptr);
		pImage->imageMemory = VK_NULL_HANDLE;
	}

	if (pImage->image != VK_NULL_HANDLE) {
		vkDestroyImage(Device, pImage->image, nullptr);
		pImage->image = VK_NULL_HANDLE;
	}
}