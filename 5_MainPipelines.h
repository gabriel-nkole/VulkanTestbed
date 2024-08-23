#pragma once
#include "main.h"


// main pipelines
bool createGraphicsPipelines(VkExtent2D swapchainExtent, VkSampleCountFlagBits msaaSampleCount, VkRenderPass renderPass, vector<VkDescriptorSetLayout>* pDescriptorSetLayouts, VkDevice device,
							 VkPipelineLayout* pGraphicsPipelineLayout, VkPipelineLayout* pLightGraphicsPipelineLayout,
							 VkPipeline* pFloorGraphicsPipeline, VkPipeline* pSkyboxGraphicsPipeline, VkPipeline* pTextureGraphicsPipeline, VkPipeline* pLightGraphicsPipeline);
bool createDescriptorResources(VkSampler sampler, VkImageView skybox_ImageView, uint32_t maxNumAlbedomaps, uint32_t numAlbedomaps, vector<Texture>* pAlbedomaps,
							   uint32_t maxNumDirLights, uint32_t maxNumSpotLights, uint32_t maxNumPointLights, Buffer numLightsBuffer, VkSampler shadowmapSampler,
							   Buffer dirLightsBuffer, Buffer dirLightVPMatricesBuffer, uint32_t numDirLights, vector<Image>* pDirShadowmaps,
							   Buffer spotLightsBuffer, Buffer spotLightVPMatricesBuffer, uint32_t numSpotLights, vector<Image>* pSpotShadowmaps,
							   Buffer pointLightsBuffer, Buffer pointLightVPMatricesBuffer, uint32_t numPointLights, vector<Image>* pPointShadowmaps,
							   VkDevice device, vector<VkDescriptorSetLayout>* pDescriptorSetLayouts, VkDescriptorPool* pDescriptorPool, vector<VkDescriptorSet>* pDescriptorSets);
bool createSampler(VkDevice device, VkSampler* pSampler);