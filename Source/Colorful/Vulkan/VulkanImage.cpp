#include "Colorful/Vulkan/VulkanImage.h"
#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanMemoryAllocator.h"

NAMESPACE_START(RHI)

VulkanImage::VulkanImage(VulkanDevice* Device, const ImageDesc& Desc)
	: VkHWObject(Device, Desc)
{
	FillAttributes(Desc);
	/// VkFormatProperties

	VkImageCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		m_VkAttributes.CreateFlags,
		m_VkAttributes.Type,
		m_VkAttributes.Format,
		VkExtent3D
		{
			Desc.Width,
			Desc.Type == EImageType::T_1D || Desc.Type == EImageType::T_1D_Array ? 1u : Desc.Height,
			Desc.Type == EImageType::T_3D ? Desc.Depth : 1u
		},
		Desc.MipLevels,
		Desc.ArrayLayers,
		VkType::SampleCount(Desc.SampleCount),
		m_VkAttributes.Tiling,
		m_VkAttributes.UsageFlags,
		m_VkAttributes.SharingMode,
		0u,
		nullptr,
		m_VkAttributes.Layout
	};
	VERIFY_VK(vkCreateImage(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));

	m_DeviceMemory = VulkanMemoryAllocator::Get().Alloc(m_Handle, EDeviceAccessFlags::GpuRead);
	VERIFY_VK(vkBindImageMemory(m_Device->Get(), m_Handle, m_DeviceMemory.Handle, 0u));

	if (Desc.Asset.Data)
	{
		auto Command = m_Device->GetOrAllocateCommandBuffer(EQueueType::Transfer, ECommandBufferLevel::Primary, true, true);

		Command->CopyBufferToImage(this, Desc.Asset.Data, Desc.Asset.Size, AllSubresource);

		if (m_Device->Options().BatchResourceSubmit)
		{
			m_Device->Queue(EQueueType::Transfer)->QueueSubmit(std::vector<ICommandBufferPtr>{Command});
		}
		else
		{
			m_Device->Queue(EQueueType::Transfer)->Submit(std::vector<ICommandBufferPtr>{Command});
		}
	}

	m_Device->SetObjectName(ToUInt64(), VK_OBJECT_TYPE_IMAGE, Desc.Name.data());
}

void VulkanImage::FillAttributes(const ImageDesc& Desc)
{
	assert(Desc.Format != EFormat::Unknown && Desc.Type != EImageType::Unknown);
	assert(Desc.ArrayLayers <= m_Device->PhysicalLimits().maxImageArrayLayers);
	assert(
		(Desc.Type == EImageType::T_1D && Desc.Width <= m_Device->PhysicalLimits().maxImageDimension1D) ||
		((Desc.Type == EImageType::T_2D || Desc.Type == EImageType::T_2D_Array) &&
			Desc.Width <= m_Device->PhysicalLimits().maxImageDimension2D &&
			Desc.Height <= m_Device->PhysicalLimits().maxImageDimension2D) ||
		((Desc.Type == EImageType::T_Cube || Desc.Type == EImageType::T_Cube_Array) &&
			Desc.Width <= m_Device->PhysicalLimits().maxImageDimensionCube &&
			Desc.Height <= m_Device->PhysicalLimits().maxImageDimensionCube) ||
		(Desc.Type == EImageType::T_3D && Desc.Height <= m_Device->PhysicalLimits().maxImageDimension3D));

	///VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME

	if (FormatAttribute::IsColor(Desc.Format))
	{
		m_VkAttributes.Aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	else if (FormatAttribute::IsDepthStenci(Desc.Format))
	{
		m_VkAttributes.Aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else if (FormatAttribute::IsDepth(Desc.Format))
	{
		m_VkAttributes.Aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else
	{
		assert(false);
	}

	if (Desc.Type == EImageType::T_Cube || Desc.Type == EImageType::T_Cube_Array)
	{
		m_VkAttributes.CreateFlags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	}

	RequiredState = Desc.RequiredState;

	m_VkAttributes.UsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	if (EnumHasAnyFlags(Desc.Usages, EBufferUsageFlags::RenderTarget))
	{
		m_VkAttributes.UsageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		RequiredState = EResourceState::RenderTarget;
	}
	if (EnumHasAnyFlags(Desc.Usages, EBufferUsageFlags::DepthStencil))
	{
		m_VkAttributes.UsageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		RequiredState = EResourceState::DepthWrite;
	}
	if (EnumHasAnyFlags(Desc.Usages, EBufferUsageFlags::UnorderedAccess))
	{
		m_VkAttributes.UsageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
		RequiredState = EResourceState::UnorderedAccess;
	}
	if (EnumHasAnyFlags(Desc.Usages, EBufferUsageFlags::ShaderResource))
	{
		m_VkAttributes.UsageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
		RequiredState = EResourceState::ShaderResource;
	}
	if (EnumHasAnyFlags(Desc.Usages, EBufferUsageFlags::InputAttachment))
	{
		/// VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT specifies that the image can be used to create a VkImageView suitable for occupying VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; 
		/// be read from a shader as an input attachment; and be used as an input attachment in a framebuffer.
		m_VkAttributes.UsageFlags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		RequiredState = EResourceState::InputAttachment;
	}

	m_VkAttributes.Type = VkType::ImageType(Desc.Type);
	m_VkAttributes.ViewType = VkType::ImageViewType(Desc.Type);
	m_VkAttributes.Format = VkType::Format(Desc.Format);

#if true
	if (Desc.RequiredState == EResourceState::Present)
	{
		PermanentState = EResourceState::Present;
	}
#endif
}

VulkanImage::VulkanImage(VulkanDevice* Device, const ImageDesc& Desc, VkImage Image, bool8_t Own)
	: VkHWObject(Device, Desc)
	, m_Own(Own)
{
	assert(Image != VK_NULL_HANDLE);
	m_Handle = Image;

	FillAttributes(Desc);
}

VkImageView VulkanImage::GetOrCrateImageView(const ImageSubresourceRange& SubresourceRange)
{
	std::lock_guard ScopedLocker(m_Views.first);

	VkImageView& ImageView = m_Views.second[SubresourceRange];
	if (ImageView == VK_NULL_HANDLE)
	{
		VkImageSubresourceRange VkSubresourceRange
		{
			m_VkAttributes.Aspect,
			SubresourceRange.BaseMipLevel,
			SubresourceRange.LevelCount == ImageSubresourceRange::AllMipLevels ? VK_REMAINING_MIP_LEVELS : SubresourceRange.LevelCount,
			SubresourceRange.BaseArrayLayer,
			SubresourceRange.LayerCount == ImageSubresourceRange::AllArrayLayers ? VK_REMAINING_ARRAY_LAYERS : SubresourceRange.LayerCount
			/// 6 * ArraySize for cubemap
		};

		VkImageViewCreateInfo CreateInfo
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0u,
			m_Handle,
			m_VkAttributes.ViewType,
			m_VkAttributes.Format,
			{
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			},
			VkSubresourceRange
		};

		VERIFY_VK(vkCreateImageView(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, &ImageView));
	}

	return ImageView;
}

VulkanImage::~VulkanImage()
{
	if (m_Own)
	{
		vkDestroyImage(m_Device->Get(), m_Handle, VK_ALLOCATION_CALLBACKS);
	}

	for (auto ImageView : m_Views.second)
	{
		vkDestroyImageView(m_Device->Get(), ImageView.second, VK_ALLOCATION_CALLBACKS);
	}

	if (m_DeviceMemory.Handle)
	{
		vkFreeMemory(m_Device->Get(), m_DeviceMemory.Handle, VK_ALLOCATION_CALLBACKS);
		m_DeviceMemory.Handle = VK_NULL_HANDLE;
	}
}

VulkanSampler::VulkanSampler(VulkanDevice* Device, const SamplerDesc& Desc)
	: VkHWObject(Device)
{
	VkSamplerCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0u,
		VkType::Filter(Desc.MinMagFilter),
		VkType::Filter(Desc.MinMagFilter),
		Desc.MinMagFilter == EFilter::Nearest ? VK_SAMPLER_MIPMAP_MODE_NEAREST : VK_SAMPLER_MIPMAP_MODE_LINEAR,
		VkType::SamplerAddressMode(Desc.AddressModeU),
		VkType::SamplerAddressMode(Desc.AddressModeV),
		VkType::SamplerAddressMode(Desc.AddressModeW),
		Desc.MipLODBias,
		Desc.MaxAnisotropy > 0 ? true : false,
		static_cast<float32_t>(Desc.MaxAnisotropy),
		false,
		VkType::CompareFunc(Desc.CompareOp),
		Desc.MinLOD,
		Desc.MaxLOD,
		VkType::BorderColor(Desc.BorderColor),
		false
	};
	VERIFY_VK(vkCreateSampler(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));
}

VulkanSampler::~VulkanSampler()
{
	vkDestroySampler(m_Device->Get(), Get(), VK_ALLOCATION_CALLBACKS);
}

NAMESPACE_END(RHI)
