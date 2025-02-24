#pragma once

#include "RHI/Vulkan/VulkanBuffer.h"

class VulkanTexture final : public VkHwResource<vk::Image>, public RHITexture
{
public:
	VulkanTexture(const class VulkanDevice& Device, const RHITextureCreateInfo& CreateInfo, RHICommandBuffer* CommandBuffer, vk::Image Image = vk::Image());

	~VulkanTexture();

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
	bool m_Own = true;
#endif
private:
	void AllocateAndBindMemory(ERHIDeviceAccessFlags AccessFlags);

	VulkanDeviceMemory m_Memory;
};

class VulkanSampler final : public VkHwResource<vk::Sampler>, public RHISampler
{
public:
	VulkanSampler(const class VulkanDevice& Device, const RHISamplerCreateInfo& CreateInfo);

	~VulkanSampler() = default;
};
