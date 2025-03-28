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
};

class VulkanGraphicsPipeline final : public VulkanPipeline, public RHIGraphicsPipeline
{
public:
	VulkanGraphicsPipeline(const class VulkanDevice& Device, vk::PipelineCache PipelineCache, const RHIGraphicsPipelineCreateInfo& CreateInfo);
};

class VulkanComputePipeline final : public VulkanPipeline
{
};

class VulkanRayTracingPipeline final : public VulkanPipeline
{
};

class VulkanPipelineState : public RHIPipelineState, public VkBaseDeviceResource
{
public:
	VulkanPipelineState(const class VulkanDevice& Device, const RHIGraphicsPipelineCreateInfo& GfxPipelineCreateInfo);

	vk::DescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout->GetNative(); }
	vk::PipelineLayout GetPipelineLayout() const { return m_PipelineLayout->GetNative(); }
private:
	void InitWrites();

	void CommitShaderResources(RHICommandBuffer* CommandBuffer) override final;
	void CommitPipelineStates(RHICommandBuffer* CommandBuffer) override final;

	std::unique_ptr<VulkanDescriptorSetLayout> m_DescriptorSetLayout;
	std::unique_ptr<VulkanPipelineLayout> m_PipelineLayout;

	std::vector<vk::DescriptorImageInfo> m_ImageInfos;
	std::vector<vk::DescriptorBufferInfo> m_BufferInfos;
	std::vector<vk::WriteDescriptorSet> m_Writes;

	vk::DescriptorSet m_Set;
};
