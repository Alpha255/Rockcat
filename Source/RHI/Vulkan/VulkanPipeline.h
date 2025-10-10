#pragma once

#include "RHI/Vulkan/VulkanAsync.h"
#include "RHI/Vulkan/VulkanDescriptor.h"
#include "RHI/Vulkan/VulkanRenderPass.h"

class VulkanPipelineCache : public VkHwResource<vk::PipelineCache>
{
public:
	VulkanPipelineCache(const class VulkanDevice& Device);

	~VulkanPipelineCache() = default;

	void Load() {}
	void Save() {}
};

class VulkanPipeline : public VkHwResource<vk::Pipeline>
{
public:
	VulkanPipeline(const class VulkanDevice& Device);

	virtual ~VulkanPipeline() = default;
protected:
	std::shared_ptr<VulkanRenderPass> m_RenderPass;
};

class VulkanGraphicsPipeline final : public VulkanPipeline, public RHIGraphicsPipeline
{
public:
	VulkanGraphicsPipeline(const class VulkanDevice& Device, vk::PipelineCache PipelineCache, const RHIGraphicsPipelineDesc& Desc);
};

class VulkanComputePipeline final : public VulkanPipeline, public RHIComputePipeline
{
public:
	VulkanComputePipeline(const class VulkanDevice& Device, vk::PipelineCache PipelineCache, const RHIComputePipelineDesc& Desc);
};

class VulkanRayTracingPipeline final : public VulkanPipeline
{
};

class VulkanPipelineState : public RHIPipelineState, public VkBaseDeviceResource
{
public:
	VulkanPipelineState(const class VulkanDevice& Device, const RHIGraphicsPipelineDesc& Desc);

	vk::DescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout->GetNative(); }
	vk::PipelineLayout GetPipelineLayout() const { return m_PipelineLayout->GetNative(); }
private:
	void CreateLayouts(const class VulkanDevice& Device, const RHIGraphicsPipelineDesc& Desc);
	void InitWriteDescriptorSets(const RHIGraphicsPipelineDesc& Desc);

	std::unique_ptr<VulkanDescriptorSetLayout> m_DescriptorSetLayout;
	std::unique_ptr<VulkanPipelineLayout> m_PipelineLayout;

	std::vector<vk::DescriptorImageInfo> m_ImageInfos;
	std::vector<vk::DescriptorBufferInfo> m_BufferInfos;
	std::vector<vk::WriteDescriptorSet> m_Writes;

	vk::DescriptorSet m_Set;
};
