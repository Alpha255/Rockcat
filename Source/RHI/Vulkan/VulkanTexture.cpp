#include "RHI/Vulkan/VulkanTexture.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanMemoryAllocator.h"
#include "RHI/RHIUploadManager.h"

VulkanTexture::VulkanTexture(const VulkanDevice& Device, const RHITextureDesc& Desc, vk::Image Image)
	: VkHwResource(Device)
	, RHITexture(Desc)
{
	assert(Desc.Format != ERHIFormat::Unknown && Desc.Dimension != ERHITextureDimension::Unknown);
	assert(Desc.NumArrayLayer <= GetDevice().GetPhysicalDeviceLimits().maxImageArrayLayers);
	assert(
		(Desc.Dimension == ERHITextureDimension::T_1D && Desc.Width <= GetDevice().GetPhysicalDeviceLimits().maxImageDimension1D) ||
		((Desc.Dimension == ERHITextureDimension::T_2D || Desc.Dimension == ERHITextureDimension::T_2D_Array) &&
			Desc.Width <= GetDevice().GetPhysicalDeviceLimits().maxImageDimension2D &&
			Desc.Height <= GetDevice().GetPhysicalDeviceLimits().maxImageDimension2D) ||
		((Desc.Dimension == ERHITextureDimension::T_Cube || Desc.Dimension == ERHITextureDimension::T_Cube_Array) &&
			Desc.Width <= GetDevice().GetPhysicalDeviceLimits().maxImageDimensionCube &&
			Desc.Height <= GetDevice().GetPhysicalDeviceLimits().maxImageDimensionCube) ||
		(Desc.Dimension == ERHITextureDimension::T_3D && Desc.Height <= GetDevice().GetPhysicalDeviceLimits().maxImageDimension3D));

	///VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME

	if (RHI::IsColor(Desc.Format))
	{
		//m_VkAttributes.Aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	else if (RHI::IsDepthStencil(Desc.Format))
	{
		//m_VkAttributes.Aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else if (RHI::IsDepth(Desc.Format))
	{
		//m_VkAttributes.Aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else
	{
		assert(false);
	}

	ERHIDeviceAccessFlags AccessFlags = ERHIDeviceAccessFlags::None;
	vk::ImageUsageFlags UsageFlags(vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst);
	if (EnumHasAnyFlags(Desc.BufferUsageFlags, ERHIBufferUsageFlags::RenderTarget))
	{
		UsageFlags |= vk::ImageUsageFlagBits::eColorAttachment;
		AccessFlags = AccessFlags | ERHIDeviceAccessFlags::GpuReadWrite;
	}
	if (EnumHasAnyFlags(Desc.BufferUsageFlags, ERHIBufferUsageFlags::DepthStencil))
	{
		UsageFlags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
		AccessFlags = AccessFlags | ERHIDeviceAccessFlags::GpuReadWrite;
	}
	if (EnumHasAnyFlags(Desc.BufferUsageFlags, ERHIBufferUsageFlags::UnorderedAccess))
	{
		UsageFlags |= vk::ImageUsageFlagBits::eStorage;
		AccessFlags = AccessFlags | ERHIDeviceAccessFlags::GpuReadWrite;
	}
	if (EnumHasAnyFlags(Desc.BufferUsageFlags, ERHIBufferUsageFlags::ShaderResource))
	{
		UsageFlags |= vk::ImageUsageFlagBits::eSampled;
		AccessFlags = AccessFlags | ERHIDeviceAccessFlags::GpuRead;
	}
	if (EnumHasAnyFlags(Desc.BufferUsageFlags, ERHIBufferUsageFlags::InputAttachment))
	{
		/// VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT specifies that the image can be used to create a VkImageView suitable for occupying VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; 
		/// be read from a shader as an input attachment; and be used as an input attachment in a framebuffer.
		UsageFlags |= vk::ImageUsageFlagBits::eInputAttachment;
		AccessFlags = AccessFlags | ERHIDeviceAccessFlags::GpuReadWrite;
	}

	if (Image)
	{
		m_Native = Image;
	}
	else
	{
		vk::ImageCreateInfo ImageCreateInfo;
		ImageCreateInfo.setFlags((Desc.Dimension == ERHITextureDimension::T_Cube || Desc.Dimension == ERHITextureDimension::T_Cube_Array) ?
				vk::ImageCreateFlagBits::eCubeCompatible : vk::ImageCreateFlags())
			.setImageType(::GetDimension(Desc.Dimension))
			.setFormat(::GetFormat(Desc.Format))
			.setExtent(vk::Extent3D(
				Desc.Width,
				Desc.Dimension == ERHITextureDimension::T_1D || Desc.Dimension == ERHITextureDimension::T_1D_Array ? 1u : Desc.Height,
				Desc.Dimension == ERHITextureDimension::T_3D ? Desc.Depth : 1u))
			.setMipLevels(Desc.NumMipLevel)
			.setArrayLayers(Desc.NumArrayLayer)
			.setSamples(::GetSampleCount(Desc.SampleCount))
			.setTiling(vk::ImageTiling::eOptimal)
			.setUsage(UsageFlags)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setInitialLayout(vk::ImageLayout::eUndefined);

		VERIFY_VK(GetNativeDevice().createImage(&ImageCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));

		AllocateAndBindMemory(AccessFlags);

		if (Desc.BulkData && Desc.BulkData->IsValid())
		{
			RHIUploadManager::Get().QueueUploadTexture(this, Desc.BulkData->GetRawData(), Desc.BulkData->GetSize(), Desc.BulkData->GetOffset());
		}
	}

	VkHwResource::SetObjectName(Desc.Name.Get().data());
}

void VulkanTexture::AllocateAndBindMemory(ERHIDeviceAccessFlags AccessFlags)
{
	m_Memory = VulkanMemoryAllocator::Get().Allocate(GetNative(), AccessFlags);
	assert(m_Memory.Native);

	/// #TODO VK_KHR_bind_memory2: On some implementations, it may be more efficient to batch memory bindings into a single command.
	GetNativeDevice().bindImageMemory(m_Native, m_Memory.Native, 0u);
}

//
//VkImageView VulkanImage::GetOrCrateImageView(const ImageSubresourceRange& SubresourceRange)
//{
//	std::lock_guard ScopedLocker(m_Views.first);
//
//	VkImageView& ImageView = m_Views.second[SubresourceRange];
//	if (ImageView == VK_NULL_HANDLE)
//	{
//		VkImageSubresourceRange VkSubresourceRange
//		{
//			m_VkAttributes.Aspect,
//			SubresourceRange.BaseMipLevel,
//			SubresourceRange.LevelCount == ImageSubresourceRange::AllMipLevels ? VK_REMAINING_MIP_LEVELS : SubresourceRange.LevelCount,
//			SubresourceRange.BaseArrayLayer,
//			SubresourceRange.LayerCount == ImageSubresourceRange::AllArrayLayers ? VK_REMAINING_ARRAY_LAYERS : SubresourceRange.LayerCount
//			/// 6 * ArraySize for cubemap
//		};
//
//		VkImageViewCreateInfo Desc
//		{
//			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
//			nullptr,
//			0u,
//			m_Handle,
//			m_VkAttributes.ViewType,
//			m_VkAttributes.Format,
//			{
//				VK_COMPONENT_SWIZZLE_R,
//				VK_COMPONENT_SWIZZLE_G,
//				VK_COMPONENT_SWIZZLE_B,
//				VK_COMPONENT_SWIZZLE_A
//			},
//			VkSubresourceRange
//		};
//
//		VERIFY_VK(vkCreateImageView(m_Device->Get(), &Desc, VK_ALLOCATION_CALLBACKS, &ImageView));
//	}
//
//	return ImageView;
//}
//

VulkanTexture::~VulkanTexture()
{
#if 0
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
#endif
}

VulkanSampler::VulkanSampler(const VulkanDevice& Device, const RHISamplerDesc& Desc)
	: VkHwResource(Device)
	, RHISampler(Desc)
{
	/*
	  If the image view has a depth/stencil format, the depth component is selected by the aspectMask,
	  and the operation is an OpImage*Dref* instruction, a depth comparison is performed.
	  The result is 1.0 if the comparison evaluates to true, and 0.0 otherwise. This value replaces the depth component D.
	  The compare operation is selected by the VkCompareOp value set by VkSamplerCreateInfo::compareOp.
	  The reference value from the SPIR-V operand Dref and the texel depth value Dtex are used as the reference and test values, respectively, in that operation.
	*/
	vk::SamplerCreateInfo SamplerCreateInfo;
	SamplerCreateInfo.setMinFilter(GetFilter(Desc.MinMagFilter))
		.setMagFilter(GetFilter(Desc.MinMagFilter))
		.setMipmapMode(Desc.MinMagFilter == ERHIFilter::Nearest ? vk::SamplerMipmapMode::eNearest : vk::SamplerMipmapMode::eLinear)
		.setAddressModeU(GetSamplerAddressMode(Desc.AddressModeU))
		.setAddressModeV(GetSamplerAddressMode(Desc.AddressModeV))
		.setAddressModeW(GetSamplerAddressMode(Desc.AddressModeW))
		.setMipLodBias(Desc.MipLODBias)
		.setAnisotropyEnable(Desc.MaxAnisotropy > 0.0f ? true : false)
		.setMaxAnisotropy(Desc.MaxAnisotropy)
		.setCompareOp(GetCompareFunc(Desc.CompareOp))
		.setCompareEnable(!(Desc.CompareOp == ERHICompareFunc::Never || Desc.CompareOp == ERHICompareFunc::Always))
		.setMinLod(Desc.MinLOD)
		.setMaxLod(Desc.MaxLOD)
		.setBorderColor(GetBorderColor(Desc.BorderColor))
		.setUnnormalizedCoordinates(false);

	VERIFY_VK(GetNativeDevice().createSampler(&SamplerCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}
