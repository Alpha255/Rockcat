#pragma once

#include "RHI/Vulkan/VulkanImage.h"

/// VulkanDescriptor Manager???

class VulkanDescriptorSetLayout final : public VkHwResource<vk::DescriptorSetLayout>
{
public:
	VulkanDescriptorSetLayout(const class VulkanDevice& Device, const RHIPipelineLayoutDesc& Desc);
};

class VulkanPipelineLayout final : public VkHwResource<vk::PipelineLayout>
{
public:
	VulkanPipelineLayout(const class VulkanDevice& Device, const RHIPipelineLayoutDesc& Desc);

	vk::DescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout.GetNative(); }
private:
	VulkanDescriptorSetLayout m_DescriptorSetLayout;
};

class VulkanDescriptorPool final : public VkHwResource<vk::DescriptorPool>
{
public:
	VulkanDescriptorPool(const class VulkanDevice& Device);

	~VulkanDescriptorPool() = default;

	void Reset();

	vk::DescriptorSet Alloc(vk::DescriptorSetLayout DescriptorSetLayout);

	bool8_t IsFull() const;
protected:
private:
	uint32_t m_AllocatedCount = 0u;
};

class VulkanDescriptorSet : public VkDeviceResource, public vk::DescriptorSet, public RHIDescriptorSet
{
public:
	VulkanDescriptorSet(const class VulkanDevice& Device, vk::PipelineLayout PipelineLayout, vk::DescriptorSetLayout DescriptorSetLayout, vk::DescriptorSet Set, const RHIPipelineLayoutDesc& Desc);

	void Commit(const RHIPipelineLayoutDesc& Desc) override final;
private:
	vk::PipelineLayout m_PipelineLayout;
	vk::DescriptorSetLayout m_DescriptorSetLayout;
};