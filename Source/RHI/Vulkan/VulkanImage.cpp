#include "RHI/Vulkan/VulkanImage.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/RHI/RHIInterface.h"

VulkanImage::VulkanImage(const VulkanDevice& Device, const RHIImageCreateInfo& CreateInfo, vk::Image Image)
	: VkHwResource(Device)
	, RHIImage(CreateInfo)
{
	assert(CreateInfo.Format != ERHIFormat::Unknown && CreateInfo.ImageType != ERHIImageType::Unknown);
	assert(CreateInfo.ArrayLayers <= GetDevice().GetPhysicalDeviceLimits().maxImageArrayLayers);
	assert(
		(CreateInfo.ImageType == ERHIImageType::T_1D && CreateInfo.Width <= GetDevice().GetPhysicalDeviceLimits().maxImageDimension1D) ||
		((CreateInfo.ImageType == ERHIImageType::T_2D || CreateInfo.ImageType == ERHIImageType::T_2D_Array) &&
			CreateInfo.Width <= GetDevice().GetPhysicalDeviceLimits().maxImageDimension2D &&
			CreateInfo.Height <= GetDevice().GetPhysicalDeviceLimits().maxImageDimension2D) ||
		((CreateInfo.ImageType == ERHIImageType::T_Cube || CreateInfo.ImageType == ERHIImageType::T_Cube_Array) &&
			CreateInfo.Width <= GetDevice().GetPhysicalDeviceLimits().maxImageDimensionCube &&
			CreateInfo.Height <= GetDevice().GetPhysicalDeviceLimits().maxImageDimensionCube) ||
		(CreateInfo.ImageType == ERHIImageType::T_3D && CreateInfo.Height <= GetDevice().GetPhysicalDeviceLimits().maxImageDimension3D));

	///VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME

	if (RHI::IsColor(CreateInfo.Format))
	{
		//m_VkAttributes.Aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	else if (RHI::IsDepthStenci(CreateInfo.Format))
	{
		//m_VkAttributes.Aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else if (RHI::IsDepth(CreateInfo.Format))
	{
		//m_VkAttributes.Aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else
	{
		assert(false);
	}

	///RequiredState = CreateInfo.RequiredState;

	vk::ImageUsageFlags UsageFlags(vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst);
	if (EnumHasAnyFlags(CreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::RenderTarget))
	{
		UsageFlags |= vk::ImageUsageFlagBits::eColorAttachment;
		//RequiredState = EResourceState::RenderTarget;
	}
	if (EnumHasAnyFlags(CreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::DepthStencil))
	{
		UsageFlags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
		//RequiredState = EResourceState::DepthWrite;
	}
	if (EnumHasAnyFlags(CreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::UnorderedAccess))
	{
		UsageFlags |= vk::ImageUsageFlagBits::eStorage;
		//RequiredState = EResourceState::UnorderedAccess;
	}
	if (EnumHasAnyFlags(CreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::ShaderResource))
	{
		UsageFlags |= vk::ImageUsageFlagBits::eSampled;
		//RequiredState = EResourceState::ShaderResource;
	}
	if (EnumHasAnyFlags(CreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::InputAttachment))
	{
		/// VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT specifies that the image can be used to create a VkImageView suitable for occupying VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; 
		/// be read from a shader as an input attachment; and be used as an input attachment in a framebuffer.
		UsageFlags |= vk::ImageUsageFlagBits::eInputAttachment;
		//RequiredState = EResourceState::InputAttachment;
	}

//#if true
//	if (CreateInfo.RequiredState == EResourceState::Present)
//	{
//		PermanentState = EResourceState::Present;
//	}
//#endif

	if (Image)
	{
		m_Native = Image;
	}
	else
	{
		auto vkCreateInfo = vk::ImageCreateInfo()
			.setFlags((CreateInfo.ImageType == ERHIImageType::T_Cube || CreateInfo.ImageType == ERHIImageType::T_Cube_Array) ?
				vk::ImageCreateFlagBits::eCubeCompatible : vk::ImageCreateFlags())
			.setImageType(::GetImageType(CreateInfo.ImageType))
			.setFormat(::GetFormat(CreateInfo.Format))
			.setExtent(vk::Extent3D(
				CreateInfo.Width,
				CreateInfo.ImageType == ERHIImageType::T_1D || CreateInfo.ImageType == ERHIImageType::T_1D_Array ? 1u : CreateInfo.Height,
				CreateInfo.ImageType == ERHIImageType::T_3D ? CreateInfo.Depth : 1u))
			.setMipLevels(CreateInfo.MipLevels)
			.setArrayLayers(CreateInfo.ArrayLayers)
			.setSamples(GetSampleCount(CreateInfo.SampleCount))
			.setTiling(vk::ImageTiling::eOptimal)
			.setUsage(UsageFlags)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setInitialLayout(vk::ImageLayout::eUndefined);
		VERIFY_VK(GetNativeDevice().createImage(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));

		if (RHIInterface::GetGraphicsSettings().BatchResourceDataTransfer)
		{
		}
		else
		{
		}
		//m_DeviceMemory = VulkanMemoryAllocator::Get().Alloc(m_Handle, EDeviceAccessFlags::GpuRead);
		//VERIFY_VK(vkBindImageMemory(m_Device->Get(), m_Handle, m_DeviceMemory.Handle, 0u));

		//if (CreateInfo.Asset.Data)
		//{
		//	auto Command = m_Device->GetOrAllocateCommandBuffer(EQueueType::Transfer, ECommandBufferLevel::Primary, true, true);

		//	Command->CopyBufferToImage(this, CreateInfo.Asset.Data, CreateInfo.Asset.Size, AllSubresource);

		//	if (m_Device->Options().BatchResourceSubmit)
		//	{
		//		m_Device->Queue(EQueueType::Transfer)->QueueSubmit(std::vector<ICommandBufferSharedPtr>{Command});
		//	}
		//	else
		//	{
		//		m_Device->Queue(EQueueType::Transfer)->Submit(std::vector<ICommandBufferSharedPtr>{Command});
		//	}
		//}
	}

	VkHwResource::SetDebugName(CreateInfo.Name.data());
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
//		VkImageViewCreateInfo CreateInfo
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
//		VERIFY_VK(vkCreateImageView(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, &ImageView));
//	}
//
//	return ImageView;
//}
//

VulkanImage::~VulkanImage()
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

VulkanSampler::VulkanSampler(const VulkanDevice& Device, const RHISamplerCreateInfo& CreateInfo)
	: VkHwResource(Device)
	, RHISampler(CreateInfo)
{
	/*
	  If the image view has a depth/stencil format, the depth component is selected by the aspectMask,
	  and the operation is an OpImage*Dref* instruction, a depth comparison is performed.
	  The result is 1.0 if the comparison evaluates to true, and 0.0 otherwise. This value replaces the depth component D.
	  The compare operation is selected by the VkCompareOp value set by VkSamplerCreateInfo::compareOp.
	  The reference value from the SPIR-V operand Dref and the texel depth value Dtex are used as the reference and test values, respectively, in that operation.
	*/
	auto vkCreateInfo = vk::SamplerCreateInfo()
		.setMinFilter(GetFilter(CreateInfo.MinMagFilter))
		.setMagFilter(GetFilter(CreateInfo.MinMagFilter))
		.setMipmapMode(CreateInfo.MinMagFilter == ERHIFilter::Nearest ? vk::SamplerMipmapMode::eNearest : vk::SamplerMipmapMode::eLinear)
		.setAddressModeU(GetSamplerAddressMode(CreateInfo.AddressModeU))
		.setAddressModeV(GetSamplerAddressMode(CreateInfo.AddressModeV))
		.setAddressModeW(GetSamplerAddressMode(CreateInfo.AddressModeW))
		.setMipLodBias(CreateInfo.MipLODBias)
		.setAnisotropyEnable(CreateInfo.MaxAnisotropy > 0.0f ? true : false)
		.setMaxAnisotropy(CreateInfo.MaxAnisotropy)
		.setCompareOp(GetCompareFunc(CreateInfo.CompareOp))
		.setCompareEnable(!(CreateInfo.CompareOp == ERHICompareFunc::Never || CreateInfo.CompareOp == ERHICompareFunc::Always))
		.setMinLod(CreateInfo.MinLOD)
		.setMaxLod(CreateInfo.MaxLOD)
		.setBorderColor(GetBorderColor(CreateInfo.BorderColor))
		.setUnnormalizedCoordinates(false);
	VERIFY_VK(GetNativeDevice().createSampler(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}
