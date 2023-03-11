#pragma once

#include "Colorful/Vulkan/VulkanBuffer.h"

NAMESPACE_START(RHI)

struct VkImageAttributes
{
	VkFormat Format = VK_FORMAT_UNDEFINED;
	VkImageCreateFlags CreateFlags = 0u;
	VkImageUsageFlags UsageFlags = 0u;
	VkImageAspectFlags Aspect = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
	VkImageType Type = VkImageType::VK_IMAGE_TYPE_2D;
	VkImageViewType ViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
	VkImageTiling Tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
	VkSharingMode SharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
	VkImageLayout Layout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
};

class VulkanImage : public VkHWObject<IImage, VkImage_T>, protected VkStatedObject
{
public:
	VulkanImage(class VulkanDevice* Device, const ImageDesc& Desc);

	VulkanImage(class VulkanDevice* Device, const ImageDesc& Desc, VkImage Image, bool8_t Own = false);

	~VulkanImage();

	VkImageView GetOrCrateImageView(const ImageSubresourceRange& SubResource);

	const VkImageAttributes& VkAttributes() const
	{
		return m_VkAttributes;
	}
protected:
	using ImageViewMap = ThreadSafeContainer<std::unordered_map<ImageSubresourceRange, VkImageView, ImageSubresourceRange::Hasher>>;
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
};

class VulkanSampler final : public VkHWObject<ISampler, VkSampler_T>
{
public:
	VulkanSampler(class VulkanDevice* Device, const SamplerDesc& Desc);

	~VulkanSampler();
protected:
private:
};

NAMESPACE_END(RHI)
