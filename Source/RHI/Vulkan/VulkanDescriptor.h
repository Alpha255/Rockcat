#pragma once

#include "RHI/Vulkan/VulkanTexture.h"

/// VulkanDescriptor Manager???

class VulkanDescriptorSetLayout final : public VkHwResource<vk::DescriptorSetLayout>
{
public:
	VulkanDescriptorSetLayout(const class VulkanDevice& Device, const RHIGraphicsPipelineDesc& Desc);
};

class VulkanPipelineLayout final : public VkHwResource<vk::PipelineLayout>
{
public:
	VulkanPipelineLayout(const class VulkanDevice& Device, const vk::DescriptorSetLayout& DescriptorSetLayout);
};

class VulkanDescriptorPool final : public VkHwResource<vk::DescriptorPool>
{
public:
	VulkanDescriptorPool(const class VulkanDevice& Device);

	~VulkanDescriptorPool() = default;

	void Reset();

	vk::DescriptorSet Allocate(vk::DescriptorSetLayout DescriptorSetLayout);
	void Free(vk::DescriptorSet Set);

	bool IsFull() const;
protected:
private:
	uint32_t m_AllocatedCount = 0u;
};