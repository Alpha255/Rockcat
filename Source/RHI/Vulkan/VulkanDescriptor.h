#pragma once

#include "RHI/Vulkan/VulkanImage.h"

/// VulkanDescriptor Manager???

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

#if 0
struct VulkanDescriptor
{
	VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
	VkDescriptorSet DescriptorSet = VK_NULL_HANDLE;

	struct KeyBindings : public std::tuple<size_t, std::vector<VkDescriptorSetLayoutBinding>, std::vector<VkPushConstantRange>>
	{
		using Super = std::tuple<size_t, std::vector<VkDescriptorSetLayoutBinding>, std::vector<VkPushConstantRange>>;
		using Super::Super;

		const size_t Hash() const
		{
			return std::get<0>(*this);
		}

		const std::vector<VkDescriptorSetLayoutBinding>& DescriptorSetLayoutBindings() const
		{
			return std::get<1>(*this);
		}

		const std::vector<VkPushConstantRange>& PushConstantRanges() const
		{
			return std::get<2>(*this);
		}
	};

	static KeyBindings MakeKeyBindings(class VulkanDevice* Device, const RHIGraphicsPipelineCreateInfo& Desc);
};
#endif