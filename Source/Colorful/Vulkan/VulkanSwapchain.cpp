#include "Colorful/Vulkan/VulkanSwapChain.h"
#include "Colorful/Vulkan/VulkanDevice.h"

NAMESPACE_START(RHI)

VulkanSurface::VulkanSurface(VulkanDevice* Device, uint64_t WindowHandle)
	: VkHWObject(Device)
{
	assert(WindowHandle);

#if defined(PLATFORM_WIN32)
	VkWin32SurfaceCreateInfoKHR CreateInfo
	{
		MEM(.sType=)VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		MEM(.pNext=)nullptr,
		MEM(.flags=)0u,  /// reserved for future use
		MEM(.hinstance=)reinterpret_cast<::HINSTANCE>(Platform::GetCurrentModuleHandle()),
		MEM(.hwnd=)reinterpret_cast<::HWND>(WindowHandle)
	};

	VERIFY_VK(vkCreateWin32SurfaceKHR(m_Device->Instance(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));
#else
	assert(0);
#endif
}

VulkanSurface::~VulkanSurface()
{
	vkDestroySurfaceKHR(m_Device->Instance(), Get(), VK_ALLOCATION_CALLBACKS);
}

VulkanSwapchain::VulkanSwapchain(
	VulkanDevice* Device,
	uint64_t WindowHandle,
	uint32_t Width,
	uint32_t Height,
	bool8_t Fullscreen,
	bool8_t VSync,
	bool8_t SRgb)
	: VkHWObject(Device)
	, m_WindowHandle(WindowHandle)
	, m_Fullscreen(Fullscreen)
	, m_VSync(VSync)
	, m_Width(Width)
	, m_Height(Height)
	, m_ColorFormat(SRgb ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM)
	, m_PresentComplete(std::move(std::make_unique<VulkanSemaphore>(Device)))
{
	Create(true);
}

void VulkanSwapchain::Create(bool8_t RecreateSurface)
{	
	if (RecreateSurface)
	{
		m_Surface = std::make_unique<VulkanSurface>(m_Device, m_WindowHandle);
	}

	uint32_t Count = 0u;
	VERIFY_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device->PhysicalDevice(), m_Surface->Get(), &Count, nullptr));
	assert(Count > 0u);
	std::vector<VkPresentModeKHR> PresentModes(Count);
	VERIFY_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device->PhysicalDevice(), m_Surface->Get(), &Count, PresentModes.data()));

	/// VK_PRESENT_MODE_IMMEDIATE_KHR: Images submitted by your application are transferred to the screen right away, which may result in tearing

	/// VK_PRESENT_MODE_FIFO_KHR: The swap chain is a queue where the display takes an image from the front of the queue when the display is 
	/// refreshed and the program inserts rendered images at the back of the queue.If the queue is full then the program has to wait.
	/// This is most similar to vertical sync as found in modern games.The moment that the display is refreshed is known as ¡°vertical blank¡±

	/// VK_PRESENT_MODE_FIFO_RELAXED_KHR: This mode only differs from the previous one if the application is late and the queue was empty at the last
	/// vertical blank.Instead of waiting for the next vertical blank, the image is transferred right away when it finally arrives.This may result in visible tearing

	/// VK_PRESENT_MODE_MAILBOX_KHR: This is another variation of the second mode. Instead of blocking the application when the queue is full, the
	/// images that are already queued are simply replaced with the newer ones. This mode can be used to implement triple buffering, which allows you 
	/// to avoid tearing with significantly less latency issues than standard vertical sync that uses double buffering
	VkPresentModeKHR PresentMode = m_VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;
	VERIFY(std::find(PresentModes.cbegin(), PresentModes.cend(), PresentMode) != PresentModes.end());

	vkGetPhysicalDeviceQueueFamilyProperties(m_Device->PhysicalDevice(), &Count, nullptr);
	assert(Count > 0u);
	std::vector<VkQueueFamilyProperties> QueueFamilyProperties(Count);
	vkGetPhysicalDeviceQueueFamilyProperties(m_Device->PhysicalDevice(), &Count, QueueFamilyProperties.data());
	std::vector<VkBool32> SurfaceSupportKHRs(Count);
	for (uint32_t QueueFamilyPropertyIndex = 0u; QueueFamilyPropertyIndex < Count; ++QueueFamilyPropertyIndex)
	{
		VERIFY_VK(vkGetPhysicalDeviceSurfaceSupportKHR(m_Device->PhysicalDevice(), QueueFamilyPropertyIndex, m_Surface->Get(), &SurfaceSupportKHRs[QueueFamilyPropertyIndex]));
	}

	uint32_t GraphicQueue = ~0u;
	uint32_t PresentQueue = ~0u;
	for (uint32_t QueueFamilyPropertyIndex = 0u; QueueFamilyPropertyIndex < Count; ++QueueFamilyPropertyIndex)
	{
		if ((EnumHasAnyFlags(QueueFamilyProperties[QueueFamilyPropertyIndex].queueFlags, VK_QUEUE_GRAPHICS_BIT)))
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
		for (uint32_t QueueFamilyPropertyIndex = 0u; QueueFamilyPropertyIndex < Count; ++QueueFamilyPropertyIndex)
		{
			if (SurfaceSupportKHRs[QueueFamilyPropertyIndex] == VK_TRUE)
			{
				PresentQueue = QueueFamilyPropertyIndex;
				break;
			}
		}
	}

	assert(GraphicQueue != ~0u && PresentQueue !=~0u && GraphicQueue == PresentQueue);

	VkSurfaceCapabilitiesKHR SurfaceCapabilities;
	VERIFY_VK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device->PhysicalDevice(), m_Surface->Get(), &SurfaceCapabilities));

	VkExtent2D SizeExtent
	{
		std::max<uint32_t>(std::min<uint32_t>(m_Width, SurfaceCapabilities.maxImageExtent.width), SurfaceCapabilities.minImageExtent.width),
		std::max<uint32_t>(std::min<uint32_t>(m_Height, SurfaceCapabilities.maxImageExtent.height), SurfaceCapabilities.minImageExtent.height),
	};

	m_Width = SizeExtent.width;
	m_Height = SizeExtent.height;

	uint32_t UsageFlagBits = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (SurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
	{
		UsageFlagBits |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}
	if (SurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		/// use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT instead and use a memory operation to transfer the rendered image to a swap chain image
		UsageFlagBits |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	VkCompositeAlphaFlagBitsKHR CompositeAlphaFlagBits = VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR;
	VkCompositeAlphaFlagBitsKHR CompositeAlphaFlagBitsArray[]
	{
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
	};
	for (auto& CompositeAlphaFlagBit : CompositeAlphaFlagBitsArray)
	{
		if (SurfaceCapabilities.supportedCompositeAlpha & CompositeAlphaFlagBit)
		{
			CompositeAlphaFlagBits = CompositeAlphaFlagBit;
			break;
		}
	}
	assert(CompositeAlphaFlagBits != VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR);

	VkSurfaceTransformFlagBitsKHR SurfaceTransformFlagBits = VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR;
	if (SurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		SurfaceTransformFlagBits = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		SurfaceTransformFlagBits = SurfaceCapabilities.currentTransform;
	}

	VERIFY_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device->PhysicalDevice(), m_Surface->Get(), &Count, nullptr));
	assert(Count > 0u);
	std::vector<VkSurfaceFormatKHR> SurfaceFormats(Count);
	VERIFY_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device->PhysicalDevice(), m_Surface->Get(), &Count, SurfaceFormats.data()));

	VkColorSpaceKHR ColorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;
	for (auto const& SurfaceFormat : SurfaceFormats)
	{
		if (SurfaceFormat.format == m_ColorFormat)
		{
			ColorSpace = SurfaceFormat.colorSpace;
			break;
		}
	}
	if (ColorSpace == VK_COLOR_SPACE_MAX_ENUM_KHR)
	{
		VkFormat ColorFormat = VK_FORMAT_UNDEFINED;
		switch (m_ColorFormat)
		{
		case VK_FORMAT_R8G8B8A8_UNORM: ColorFormat = VK_FORMAT_B8G8R8A8_UNORM; break;
		case VK_FORMAT_B8G8R8A8_UNORM: ColorFormat = VK_FORMAT_R8G8B8A8_UNORM; break;
		case VK_FORMAT_B8G8R8A8_SRGB:  ColorFormat = VK_FORMAT_R8G8B8A8_SRGB;  break;
		case VK_FORMAT_R8G8B8A8_SRGB:  ColorFormat = VK_FORMAT_B8G8R8A8_SRGB;  break;
		}

		if (ColorFormat != VK_FORMAT_UNDEFINED)
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

		assert(ColorSpace != VK_COLOR_SPACE_MAX_ENUM_KHR);
	}

	/// Specify how to handle swap chain images that will be used across multiple queue families, 
	/// That will be the case in application if the graphics queue family is different from the presentation queue

	/// VK_SHARING_MODE_EXCLUSIVE: An image is owned by one queue family at a time and ownership must be explicitly transfered before using 
	/// it in another queue family.This option offers the best performance

	/// VK_SHARING_MODE_CONCURRENT: Images can be used across multiple queue families without explicit ownership transfers,
	/// concurrent mode requires you to specify at least two distinct queue families
	uint32_t MinImageCount = std::min<uint32_t>(SurfaceCapabilities.minImageCount + 1u, SurfaceCapabilities.maxImageCount);
	VkSwapchainKHR OldSwapchain = m_Handle;
	VkSwapchainCreateInfoKHR CreateInfo
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0u,
		m_Surface->Get(),
		PresentMode == VK_PRESENT_MODE_MAILBOX_KHR ? MinImageCount : SurfaceCapabilities.minImageCount,
		m_ColorFormat,
		ColorSpace,
		SizeExtent,
		1u, /// This is always 1 unless you are developing a stereoscopic 3D application
		UsageFlagBits,
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr,
		SurfaceTransformFlagBits,
		CompositeAlphaFlagBits,  /// The compositeAlpha field specifies if the alpha channel should be used for blending with other windows in the window system
		PresentMode,
		VK_TRUE, /// clipped specifies whether the Vulkan implementation is allowed to discard rendering operations that affect regions of the surface that are not visible.
		OldSwapchain
	};

	/// If the clipped member is set to VK_TRUE then that means that we don't care about the color of pixels that are
	/// obscured, for example because another window is in front of them.Unless you really need to be able to read these 
	/// pixels back and get predictable results, you will get the best performance by enabling clipping
	
#if VK_EXT_full_screen_exclusive
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
	if (m_Device->EnabledExtensions().FullscreenExclusive)
	{
		VkSurfaceFullScreenExclusiveInfoEXT FullscreenExclusiveInfo
		{
			VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT,
			nullptr,
			m_Fullscreen ? VK_FULL_SCREEN_EXCLUSIVE_ALLOWED_EXT : VK_FULL_SCREEN_EXCLUSIVE_DISALLOWED_EXT
		};

		///FullscreenExclusiveInfo.pNext = const_cast<void*>(CreateInfo.pNext);
		CreateInfo.pNext = &FullscreenExclusiveInfo;
	}
#endif

	VERIFY_VK(vkCreateSwapchainKHR(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));

	if (OldSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(m_Device->Get(), OldSwapchain, VK_ALLOCATION_CALLBACKS);
		OldSwapchain = VK_NULL_HANDLE;
	}

	VERIFY_VK(vkGetSwapchainImagesKHR(m_Device->Get(), Get(), &Count, nullptr));

	std::vector<VkImage> Images(Count);
	VERIFY_VK(vkGetSwapchainImagesKHR(m_Device->Get(), Get(), &Count, Images.data()));

	m_BackBuffers.resize(Count);
	for (uint32_t ImageIndex = 0u; ImageIndex < Count; ++ImageIndex)
	{
		auto Image = std::make_shared<VulkanImage>(
			m_Device, 
			ImageDesc
			{
				m_Width,
				m_Height,
				1u,
				1u,
				1u,
				EImageType::T_2D,
				FormatAttribute::Attribute_Vk(m_ColorFormat).Format,
				ESampleCount::Sample_1_Bit,
				EBufferUsageFlags::None,
				EResourceState::Present,
				String::Format("SwapchainImage-%d", ImageIndex)
			}, 
			Images[ImageIndex]);

		FrameBufferDesc Desc;
		Desc.AddColorAttachment(Image);

		m_BackBuffers[ImageIndex] = std::make_shared<VulkanFramebuffer>(m_Device, Desc);
	}
}

void VulkanSwapchain::Recreate()
{
	VkSurfaceCapabilitiesKHR SurfaceCapabilities;
	if (VK_ERROR_SURFACE_LOST_KHR == vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device->PhysicalDevice(), m_Surface->Get(), &SurfaceCapabilities))
	{
		if (IsValid())
		{
			vkDestroySwapchainKHR(m_Device->Get(), Get(), VK_ALLOCATION_CALLBACKS);
			m_Handle = VK_NULL_HANDLE;

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
	assert(m_Handle != VK_NULL_HANDLE);

	auto Ret = vkAcquireNextImageKHR(
		m_Device->Get(),
		m_Handle,
		std::numeric_limits<uint64_t>::max(),  /// Wait infinite ???
		m_PresentComplete->Get(),              /// Signal present complete semaphore
		VK_NULL_HANDLE,                        /// No fence to signal???
		&m_CurImageIndex);

	/// VK_SUBOPTIMAL_KHR: A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully.
	assert(m_CurImageIndex < m_BackBuffers.size());

	if (!(Ret == VK_SUCCESS || Ret == VK_SUBOPTIMAL_KHR))
	{
		if (Ret == VK_ERROR_OUT_OF_DATE_KHR || Ret == VK_ERROR_SURFACE_LOST_KHR)
		{
			m_Device->WaitIdle();
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
	assert(m_Handle != VK_NULL_HANDLE);

#if false
	m_Device->Queue(EQueueType::Graphics)->AddSignalSemaphore(m_PresentComplete->get());
#endif

	const VkSemaphore WaitSemaphores[]
	{
		m_PresentComplete->Get()
	};

	const VkSwapchainKHR Swapchains[]
	{
		m_Handle
	};

	VkQueue PresentQueue = m_Device->Queue(EQueueType::Graphics)->Get();

	/// Before an application can present an image, the image¡¯s layout must be transitioned to the VK_IMAGE_LAYOUT_PRESENT_SRC_KHR layout, 
	/// or for a shared presentable image the VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR layout.
	VkPresentInfoKHR PresentInfo
	{
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1u,
		WaitSemaphores,  /// Wait vkAcquireNextImageKHR to complete
		1u,
		Swapchains,
		&m_CurImageIndex,
		nullptr
	};

	auto Ret = vkQueuePresentKHR(PresentQueue, &PresentInfo);
	if (!(Ret == VK_SUCCESS || Ret == VK_SUBOPTIMAL_KHR))
	{
		if (Ret == VK_ERROR_OUT_OF_DATE_KHR || Ret == VK_ERROR_SURFACE_LOST_KHR)
		{
			m_Device->WaitIdle();
			Recreate();
		}
		else
		{
			assert(false);
		}
	}
}

VulkanSwapchain::~VulkanSwapchain()
{
	vkDestroySwapchainKHR(m_Device->Get(), Get(), VK_ALLOCATION_CALLBACKS);
}

NAMESPACE_END(RHI)