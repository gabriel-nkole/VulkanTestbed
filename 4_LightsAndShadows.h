#pragma once
#include "main.h"


// lights & shadows
bool createShadowmapGraphicsPipelines(VkExtent2D shadowmapExtent, VkRenderPass dirSpotShadowmapRenderPass, VkRenderPass pointShadowmapRenderPass, VkDevice device,
									  VkPipelineLayout* pDirSpotShadowmapGraphicsPipelineLayout, VkPipeline* pDirSpotShadowmapGraphicsPipeline,
									  VkDescriptorSetLayout* pPointShadowmapDescriptorSetLayout, VkPipelineLayout* pPointShadowmapGraphicsPipelineLayout, VkPipeline* pPointShadowmapGraphicsPipeline);
bool createPointShadowmapDescriptorResources(Buffer numLightsBuffer, Buffer pointLightVPMatricesBuffer, Buffer pointLightsBuffer,
											 VkDevice device, VkDescriptorSetLayout* pPointShadowmapDescriptorSetLayout, VkDescriptorPool* pPointShadowmapDescriptorPool, VkDescriptorSet* pPointShadowmapDescriptorSet);
bool createShadowmapResources(VkCommandPool graphicsCommandPool, VkQueue graphicsQueue, VkPhysicalDevice physicalDevice, VkDevice device,
							  VkSampler* pShadowmapSampler, VkRenderPass* pDirSpotShadowmapRenderPass, VkRenderPass* pPointShadowmapRenderPass,
							  VkExtent2D shadowmapExtent, uint32_t numDirLights, vector<Image>* pDirShadowmaps, uint32_t numSpotLights, vector<Image>* pSpotShadowmaps,
							  uint32_t numPointLights, vector<Image>* pPointShadowmaps);

bool createLightModelsAndObjects(VkQueue graphicsQueue, VkCommandPool graphicsCommandPool, VkPhysicalDevice physicalDevice, VkDevice device,
								 vector<Light>* pDirLights, vector<Light>* pSpotLights, vector<Light>* pPointLights,
								 vector<Model>* pLightModels, vector<Object>* pDirLightObjects, vector<Object>* pSpotLightObjects, vector<Object>* pPointLightObjects);
bool createLightBuffers(VkExtent2D shadowmapExtent, VkPhysicalDevice physicalDevice, VkDevice device, Buffer* pNumLightsBuffer,
						uint32_t maxNumDirLights, vector<Light>* pDirLights, Buffer* pDirLightsBuffer, vector<XMMATRIX>* pDirLightVPMatrices, Buffer* pDirLightVPMatricesBuffer,
						uint32_t maxNumSpotLights, vector<Light>* pSpotLights, Buffer* pSpotLightsBuffer, vector<XMMATRIX>* pSpotLightVPMatrices, Buffer* pSpotLightVPMatricesBuffer,
						uint32_t maxNumPointLights, vector<Light>* pPointLights, Buffer* pPointLightsBuffer, vector<XMMATRIX>* pPointLightVPMatrices, Buffer* pPointLightVPMatricesBuffer);
XMFLOAT3 getDir(float rotX, float rotY);