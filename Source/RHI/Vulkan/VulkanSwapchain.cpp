#include "RHI/Vulkan/VulkanSwapChain.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanRHI.h"
#include "RHI/Vulkan/VulkanLayerExtensions.h"
#include "Core/PlatformMisc.h"
#include "Engine/Services/SpdLogService.h"

VulkanSurface::VulkanSurface(const VulkanDevice& Device, const void* WindowHandle)
	: VkDeviceResource(Device)
{
	assert(WindowHandle);

#if defined(PLATFORM_WIN32)
	auto vkCreateInfo = vk::Win32SurfaceCreateInfoKHR()
		.setHinstance(reinterpret_cast<::HINSTANCE>(PlatformMisc::GetCurrentModuleHandle()))
		.setHwnd(reinterpret_cast<::HWND>(const_cast<void*>(WindowHandle)));
	VERIFY_VK(GetNativeInstance().createWin32SurfaceKHR(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
#else
	assert(0);
#endif
}

VulkanSurface::~VulkanSurface()
{
	GetNativeInstance().destroy(m_Native);
	m_Native = nullptr;
}

VulkanSwapchain::VulkanSwapchain(const VulkanDevice& Device, const void* WindowHandle, uint32_t Width, uint32_t Height)
	: VkHwResource(Device)
	, m_VSync(VulkanRHI::GetGraphicsSettings().VSync)
	, m_Fullscreen(VulkanRHI::GetGraphicsSettings().FullScreen)
	, m_WindowHandle(WindowHandle)
	, m_Width(Width)
	, m_Height(Height)
	, m_ColorFormat(VulkanRHI::GetGraphicsSettings().SRGBSwapchain ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8G8B8A8Unorm)
	//, m_PresentComplete(std::move(std::make_unique<VulkanSemaphore>(Device)))
{
	Create(true);
}

void VulkanSwapchain::Create(bool RecreateSurface)
{	
	if (RecreateSurface)
	{
		m_Surface = std::make_unique<VulkanSurface>(GetDevice(), m_WindowHandle);
	}

	/// VK_PRESENT_MODE_IMMEDIATE_KHR: Images submitted by your application are transferred to the screen right away, which may result in tearing

	/// VK_PRESENT_MODE_FIFO_KHR: The swap chain is a queue where the display takes an image from the front of the queue when the display is 
	/// refreshed and the program inserts rendered images at the back of the queue.If the queue is full then the program has to wait.
	/// This is most similar to vertical sync as found in modern games.The moment that the display is refreshed is known as ��vertical blank��

	/// VK_PRESENT_MODE_FIFO_RELAXED_KHR: This mode only differs from the previous one if the application is late and the queue was empty at the last
	/// vertical blank.Instead of waiting for the next vertical blank, the image is transferred right away when it finally arrives.This may result in visible tearing

	/// VK_PRESENT_MODE_MAILBOX_KHR: This is another variation of the second mode. Instead of blocking the application when the queue is full, the
	/// images that are already queued are simply replaced with the newer ones. This mode can be used to implement triple buffering, which allows you 
	/// to avoid tearing with significantly less latency issues than standard vertical sync that uses double buffering
	
	auto PresentModes = GetNativePhysicalDevice().getSurfacePresentModesKHR(m_Surface->GetNative());
	vk::PresentModeKHR PresentMode = m_VSync ? vk::PresentModeKHR::eFifo : vk::PresentModeKHR::eMailbox;
	VERIFY(std::find(PresentModes.cbegin(), PresentModes.cend(), PresentMode) != PresentModes.end());

	auto QueueFamilyProperties = GetNativePhysicalDevice().getQueueFamilyProperties();
	std::vector<VkBool32> SurfaceSupportKHRs(QueueFamilyProperties.size());
	for (uint32_t QueueFamilyPropertyIndex = 0u; QueueFamilyPropertyIndex < QueueFamilyProperties.size(); ++QueueFamilyPropertyIndex)
	{
		VERIFY_VK(GetNativePhysicalDevice().getSurfaceSupportKHR(QueueFamilyPropertyIndex, m_Surface->GetNative(), &SurfaceSupportKHRs[QueueFamilyPropertyIndex]));
	}

	uint32_t GraphicQueue = ~0u;
	uint32_t PresentQueue = ~0u;
	for (uint32_t QueueFamilyPropertyIndex = 0u; QueueFamilyPropertyIndex < QueueFamilyProperties.size(); ++QueueFamilyPropertyIndex)
	{
		if ((QueueFamilyProperties[QueueFamilyPropertyIndex].queueFlags & vk::QueueFlagBits::eGraphics))
		{
			GraphicQueue = GraphicQueue == ~0u ? QueueFamilyPropertyIndex : GraphicQueue;

			if (SurfaceSupportKHRs[QueueFamilyPropertyIndex] == VK_TRUE)
			{
				GraphicQueue = QueueFamilyPropertyIndex;
				PresentQueue = QueueFamilyPropertyIndex;
				break;
			}
		}
	}
	if (PresentQueue == ~0u)
	{
		for (uint32_t QueueFamilyPropertyIndex = 0u; QueueFamilyPropertyIndex < QueueFamilyProperties.size(); ++QueueFamilyPropertyIndex)
		{
			if (SurfaceSupportKHRs[QueueFamilyPropertyIndex] == VK_TRUE)
			{
				PresentQueue = QueueFamilyPropertyIndex;
				break;
			}
		}
	}

	assert(GraphicQueue != ~0u && PresentQueue !=~0u && GraphicQueue == PresentQueue);

	auto SurfaceCapabilities = GetNativePhysicalDevice().getSurfaceCapabilitiesKHR(m_Surface->GetNative());

	vk::Extent2D Extent
	{
		std::max<uint32_t>(std::min<uint32_t>(m_Width, SurfaceCapabilities.maxImageExtent.width), SurfaceCapabilities.minImageExtent.width),
		std::max<uint32_t>(std::min<uint32_t>(m_Height, SurfaceCapabilities.maxImageExtent.height), SurfaceCapabilities.minImageExtent.height),
	};

	m_Width = Extent.width;
	m_Height = Extent.height;

	vk::ImageUsageFlags UsageFlags(vk::ImageUsageFlagBits::eColorAttachment);
	if (SurfaceCapabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferSrc)
	{
		UsageFlags |= vk::ImageUsageFlagBits::eTransferSrc;
	}
	if (SurfaceCapabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferDst)
	{
		/// use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT instead and use a memory operation to transfer the rendered image to a swap chain image
		UsageFlags |= vk::ImageUsageFlagBits::eTransferDst;
	}

	auto CompositeAlphaFlagBits = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	vk::CompositeAlphaFlagBitsKHR CompositeAlphaFlagBitsArray[]
	{
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
		vk::CompositeAlphaFlagBitsKHR::ePostMultiplied,
		vk::CompositeAlphaFlagBitsKHR::eInherit
	};
	for (auto& CompositeAlphaFlagBit : CompositeAlphaFlagBitsArray)
	{
		if (SurfaceCapabilities.supportedCompositeAlpha & CompositeAlphaFlagBit)
		{
			CompositeAlphaFlagBits = CompositeAlphaFlagBit;
			break;
		}
	}

	auto SurfaceTransformFlagBits = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	if (SurfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
	{
		SurfaceTransformFlagBits = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	}
	else
	{
		SurfaceTransformFlagBits = SurfaceCapabilities.currentTransform;
	}

	auto SurfaceFormats = GetNativePhysicalDevice().getSurfaceFormatsKHR(m_Surface->GetNative());

	vk::ColorSpaceKHR ColorSpace = static_cast<vk::ColorSpaceKHR>(VkColorSpaceKHR::VK_COLOR_SPACE_MAX_ENUM_KHR);
	for (auto const& SurfaceFormat : SurfaceFormats)
	{
		if (SurfaceFormat.format == m_ColorFormat)
		{
			ColorSpace = SurfaceFormat.colorSpace;
			break;
		}
	}
	if (ColorSpace == static_cast<vk::ColorSpaceKHR>(VkColorSpaceKHR::VK_COLOR_SPACE_MAX_ENUM_KHR))
	{
		vk::Format ColorFormat = vk::Format::eUndefined;
		switch (m_ColorFormat)
		{
		case vk::Format::eR8G8B8A8Unorm:
		case vk::Format::eR8G8B8A8Srgb:
		case vk::Format::eB8G8R8A8Unorm:
		case vk::Format::eB8G8R8A8Srgb:
			ColorFormat = m_ColorFormat;
		}

		if (ColorFormat != vk::Format::eUndefined)
		{
			for (auto const& SurfaceFormat : SurfaceFormats)
			{
				if (SurfaceFormat.format == ColorFormat)
				{
					ColorSpace = SurfaceFormat.colorSpace;
					m_ColorFormat = ColorFormat;
					break;
				}
			}
		}

		assert(ColorSpace != static_cast<vk::ColorSpaceKHR>(VkColorSpaceKHR::VK_COLOR_SPACE_MAX_ENUM_KHR));
	}

	/// Specify how to handle swap chain images that will be used across multiple queue families, 
	/// That will be the case in application if the graphics queue family is different from the presentation queue

	/// VK_SHARING_MODE_EXCLUSIVE: An image is owned by one queue family at a time and ownership must be explicitly transfered before using 
	/// it in another queue family.This option offers the best performance

	/// VK_SHARING_MODE_CONCURRENT: Images can be used across multiple queue families without explicit ownership transfers,
	/// concurrent mode requires you to specify at least two distinct queue families
	vk::SwapchainKHR OldSwapchain = m_Native;
	auto vkCreateInfo = vk::SwapchainCreateInfoKHR()
		.setOldSwapchain(m_Native)
		.setSurface(m_Surface->GetNative())
		.setMinImageCount(PresentMode == vk::PresentModeKHR::eMailbox ? std::min<uint32_t>(SurfaceCapabilities.minImageCount + 1u, SurfaceCapabilities.maxImageCount) : SurfaceCapabilities.minImageCount)
		.setImageFormat(m_ColorFormat)
		.setImageColorSpace(ColorSpace)
		.setImageExtent(Extent)
		.setImageArrayLayers(1u)                           /// This is always 1 unless you are developing a stereoscopic 3D application
		.setImageUsage(UsageFlags)
		.setImageSharingMode(vk::SharingMode::eExclusive)
		.setPreTransform(SurfaceTransformFlagBits)
		.setCompositeAlpha(CompositeAlphaFlagBits)         /// The compositeAlpha field specifies if the alpha channel should be used for blending with other windows in the window system
		.setPresentMode(PresentMode)
		.setClipped(true);                                 /// clipped specifies whether the Vulkan implementation is allowed to discard rendering operations that affect regions of the surface that are not visible.

	/// If the clipped member is set to VK_TRUE then that means that we don't care about the color of pixels that are
	/// obscured, for example because another window is in front of them.Unless you really need to be able to read these 
	/// pixels back and get predictable results, you will get the best performance by enabling clipping
	
	/****************************************************************************************************************************************************************
		VK_FULL_SCREEN_EXCLUSIVE_DEFAULT_EXT indicates the implementation should determine the appropriate full-screen method by whatever means it deems appropriate.

		VK_FULL_SCREEN_EXCLUSIVE_ALLOWED_EXT indicates the implementation may use full-screen exclusive mechanisms when available. 
		Such mechanisms may result in better performance and/or the availability of different presentation capabilities, but may require a more disruptive transition during swapchain initialization, first presentation and/or destruction.

		VK_FULL_SCREEN_EXCLUSIVE_DISALLOWED_EXT indicates the implementation should avoid using full-screen mechanisms which rely on disruptive transitions.

		VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT indicates the application will manage full-screen exclusive mode by using the vkAcquireFullScreenExclusiveModeEXT and vkReleaseFullScreenExclusiveModeEXT commands.
		VK_FULL_SCREEN_EXCLUSIVE_DEFAULT_EXT indicates the implementation should determine the appropriate full-screen method by whatever means it deems appropriate.

		VK_FULL_SCREEN_EXCLUSIVE_ALLOWED_EXT indicates the implementation may use full-screen exclusive mechanisms when available. 
		Such mechanisms may result in better performance and/or the availability of different presentation capabilities, 
		but may require a more disruptive transition during swapchain initialization, first presentation and/or destruction.

		VK_FULL_SCREEN_EXCLUSIVE_DISALLOWED_EXT indicates the implementation should avoid using full-screen mechanisms which rely on disruptive transitions.

		VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT indicates the application will manage full-screen exclusive mode by using the vkAcquireFullScreenExclusiveModeEXT and vkReleaseFullScreenExclusiveModeEXT commands.
	*******************************************************************************************************************************************************************/
	if (VulkanRHI::GetLayerExtensionConfigs().HasFullscreenExclusive)
	{
		auto FullscreenExclusiveInfo = vk::SurfaceFullScreenExclusiveInfoEXT()
			.setFullScreenExclusive(m_Fullscreen ? vk::FullScreenExclusiveEXT::eAllowed : vk::FullScreenExclusiveEXT::eDisallowed);
		SetPNext(vkCreateInfo, FullscreenExclusiveInfo);
	}

	VERIFY_VK(GetNativeDevice().createSwapchainKHR(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));

	if (OldSwapchain)
	{
		GetNativeDevice().destroy(OldSwapchain, VK_ALLOCATION_CALLBACKS);
		OldSwapchain = nullptr;
	}

	auto Images = GetNativeDevice().getSwapchainImagesKHR(m_Native);

	//m_BackBuffers.resize(Count);
	for (uint32_t ImageIndex = 0u; ImageIndex < Images.size(); ++ImageIndex)
	{
		auto CreateInfo = RHITextureCreateInfo()
			.SetWidth(m_Width)
			.SetHeight(m_Height)
			.SetDepth(1u)
			.SetArrayLayers(1u)
			.SetMipLevels(1u)
			.SetDimension(ERHITextureDimension::T_2D)
			.SetFormat(RHI::GetRHIFormat(m_ColorFormat))
			.SetSampleCount(ERHISampleCount::Sample_1_Bit)
			.SetUsages(ERHIBufferUsageFlags::None)
			.SetName(StringUtils::Format("SwapchainImage-%d", ImageIndex));
		auto Image = std::make_shared<VulkanTexture>(GetDevice(), CreateInfo, Images[ImageIndex]);

		//FrameBufferDesc Desc;
		//Desc.AddColorAttachment(Image);

		//m_BackBuffers[ImageIndex] = std::make_shared<VulkanFramebuffer>(m_Device, Desc);
	}
}

void VulkanSwapchain::Recreate()
{
	vk::SurfaceCapabilitiesKHR SurfaceCapabilities;
	if (vk::Result::eErrorSurfaceLostKHR == GetNativePhysicalDevice().getSurfaceCapabilitiesKHR(m_Surface->GetNative(), &SurfaceCapabilities))
	{
		if (m_Native)
		{
			GetNativeDevice().destroy(m_Native, VK_ALLOCATION_CALLBACKS);
			m_Native = nullptr;

			Create(true);
		}
	}
	else
	{
		Create(false);
	}
}

void VulkanSwapchain::AcquireNextImage()
{
	assert(m_Native);

	auto Result = GetNativeDevice().acquireNextImageKHR(
		m_Native,
		std::numeric_limits<uint64_t>::max(),  /// Wait infinite ???
		//m_PresentComplete->Get(),            /// Signal present complete semaphore
		nullptr,
		nullptr,                               /// No fence to signal???
		&m_CurImageIndex);

	/// VK_SUBOPTIMAL_KHR: A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully.
	// assert(m_CurImageIndex < m_BackBuffers.size());

	if (!(Result == vk::Result::eSuccess || Result == vk::Result::eSuboptimalKHR))
	{
		if (Result == vk::Result::eErrorOutOfDateKHR || Result == vk::Result::eErrorSurfaceLostKHR)
		{
			GetDevice().WaitIdle();
			Recreate();
		}
		else
		{
			assert(false);
		}
	}

#if false
	m_Device->Queue(EQueueType::Graphics)->AddWaitSemaphore(m_PresentComplete->get());
#endif
}

void VulkanSwapchain::Present()
{
	assert(m_Native);

#if false
	m_Device->Queue(EQueueType::Graphics)->AddSignalSemaphore(m_PresentComplete->get());
#endif

	const vk::Semaphore WaitSemaphores[]
	{
		//m_PresentComplete->Get()
		nullptr
	};

	const vk::SwapchainKHR Swapchains[]
	{
		m_Native
	};

	//vk::Queue PresentQueue = m_Device->Queue(EQueueType::Graphics)->Get();
	vk::Queue PresentQueue;

	/// Before an application can present an image, the image��s layout must be transitioned to the VK_IMAGE_LAYOUT_PRESENT_SRC_KHR layout, 
	/// or for a shared presentable image the VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR layout.
	auto PresentInfo = vk::PresentInfoKHR()
		.setWaitSemaphores(WaitSemaphores)
		.setSwapchains(Swapchains)
		.setPImageIndices(&m_CurImageIndex);

	auto Result = PresentQueue.presentKHR(&PresentInfo);
	if (!(Result == vk::Result::eSuccess || Result == vk::Result::eSuboptimalKHR))
	{
		if (Result == vk::Result::eErrorOutOfDateKHR || Result == vk::Result::eErrorSurfaceLostKHR)
		{
			GetDevice().WaitIdle();
			Recreate();
		}
		else
		{
			assert(false);
		}
	}
}