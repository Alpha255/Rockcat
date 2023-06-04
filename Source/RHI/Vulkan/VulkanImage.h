#pragma once

#include "RHI/Vulkan/VulkanBuffer.h"

class VulkanImage final : public VkHwResource<vk::Image>, public RHIImage
{
public:
	VulkanImage(const class VulkanDevice& Device, const RHIImageCreateInfo& CreateInfo, vk::Image Image = vk::Image());

	~VulkanImage();

#if 0
	VkImageView GetOrCrateImageView(const ImageSubresourceRange& SubResource);
protected:
	using ImageViewMap = std::pair<std::mutex, std::unordered_map<ImageSubresourceRange, VkImageView, ImageSubresourceRange::Hasher>>;
	friend struct VulkanPipelineBarrier;
	friend class VulkanCommandBuffer;
	friend class VulkanGraphicsPipelineState;

	void SetImageLayout(VkImageLayout Layout)
	{
		m_VkAttributes.Layout = Layout;
	}

	void FillAttributes(const ImageDesc& Desc);
private:
	VkImageAttributes m_VkAttributes;
	VulkanDeviceMemory m_DeviceMemory;
	ImageViewMap m_Views;
	bool8_t m_Own = true;
#endif
};

class VulkanSampler final : public VkHwResource<vk::Sampler>, public RHISampler
{
public:
	VulkanSampler(const class VulkanDevice& Device, const RHISamplerCreateInfo& CreateInfo);

	~VulkanSampler() = default;
};
