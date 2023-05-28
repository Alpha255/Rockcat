#pragma once

#include "Colorful/Vulkan/VulkanAsync.h"
#include "Colorful/Vulkan/VulkanDescriptor.h"
#include "Colorful/Vulkan/VulkanRenderPass.h"

NAMESPACE_START(RHI)

class VulkanPipelineCache : public VkHWObject<void, VkPipelineCache_T>
{
public:
	VulkanPipelineCache(class VulkanDevice* Device);

	~VulkanPipelineCache();

	void Load() {}
	void Save() {}
};

class VulkanPipeline : public VkHWObject<IPipeline, VkPipeline_T>
{
public:
	VulkanPipeline(class VulkanDevice* Device);

	virtual ~VulkanPipeline();

	VulkanDescriptor& Descriptor()
	{
		return m_Descriptor;
	}
protected:
	VulkanDescriptor m_Descriptor;
};

class VulkanGraphicsPipeline final : public VulkanPipeline
{
public:
	VulkanGraphicsPipeline(class VulkanDevice* Device, VkPipelineCache PipelineCache, const GraphicsPipelineDesc& Desc);
protected:
private:
};

class VulkanComputePipeline final : public VulkanPipeline
{
};

class VulkanRayTracingPipeline final : public VulkanPipeline
{
};

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

NAMESPACE_END(RHI)
