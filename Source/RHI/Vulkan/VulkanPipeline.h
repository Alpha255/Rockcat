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

	virtual ~VulkanPipeline();

//	VulkanDescriptor& Descriptor()
//	{
//		return m_Descriptor;
//	}
//protected:
//	VulkanDescriptor m_Descriptor;
};

class VulkanGraphicsPipeline final : public VulkanPipeline
{
public:
	VulkanGraphicsPipeline(const class VulkanDevice& Device, vk::PipelineCache PipelineCache, const RHIGraphicsPipelineCreateInfo& CreateInfo);
protected:
private:
};

class VulkanComputePipeline final : public VulkanPipeline
{
};

class VulkanRayTracingPipeline final : public VulkanPipeline
{
};

#if 0
class VulkanGraphicsPipelineState : public PipelineState
{
public:
	VulkanGraphicsPipelineState(const GraphicsPipelineDesc& Desc, VkDescriptorSet DescriptorSet);

	const std::vector<VkWriteDescriptorSet>& WriteDescriptorSets() const
	{
		return m_Writes;
	}

	struct ResourceNeedTransitionState
	{
		class VulkanImage* Image = nullptr;
		class VulkanBuffer* Buffer = nullptr;
	};

	std::vector<ResourceNeedTransitionState> ResourcesNeedTransitionState;
protected:
	void WriteImage(EShaderStage Stage, uint32_t Binding, IImage* Image) override final;
	void WriteSampler(EShaderStage Stage, uint32_t Binding, ISampler* Sampler) override final;
	void WriteUniformBuffer(EShaderStage Stage, uint32_t Binding, IBuffer* Buffer) override final;
private:
	std::vector<VkWriteDescriptorSet> m_Writes;
	std::list<VkDescriptorImageInfo> m_ImageInfos;
	std::list<VkDescriptorBufferInfo> m_BufferInfos;
	std::list<VkBufferView> m_BufferViews;
	VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
};

#endif
