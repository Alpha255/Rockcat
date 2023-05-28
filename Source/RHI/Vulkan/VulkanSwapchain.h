#pragma once

#include "Colorful/Vulkan/VulkanAsync.h"
#include "Colorful/Vulkan/VulkanImage.h"

NAMESPACE_START(RHI)

class VulkanSurface final : public VkHWObject<void, VkSurfaceKHR_T>
{
public:
	VulkanSurface(class VulkanDevice* Device, uint64_t WindowHandle);

	~VulkanSurface();
};

class VulkanSwapchain final : public VkHWObject<void, VkSwapchainKHR_T>
{
public:
	VulkanSwapchain(
		class VulkanDevice* Device,
		uint64_t WindowHandle, 
		uint32_t Width,
		uint32_t Height,
		bool8_t Fullscreen,
		bool8_t VSync,
		bool8_t SRgb);

	~VulkanSwapchain();

	void Resize(uint32_t Width, uint32_t Height)
	{
		if ((Width && Height) && (m_Width != Width || m_Height != Height))
		{
			m_Width = Width;
			m_Height = Height;
		}
	}

	inline uint32_t Width() const
	{
		return m_Width;
	}

	inline uint32_t Height() const
	{
		return m_Height;
	}

	inline uint32_t CurrentImageIndex() const
	{
		return m_CurImageIndex;
	}

	inline VkFormat ColorFormat() const
	{
		return m_ColorFormat;
	}

	VkSemaphore PresentCompleteSemaphore() const
	{
		return m_PresentComplete->Get();
	}

	class VulkanFramebuffer* CurrentFrameBuffer() const
	{
		return m_BackBuffers[m_CurImageIndex].get();
	}

	void AcquireNextImage();

	void Present();
protected:
	void Create(bool8_t RecreateSurface);

	void Recreate();
private:
	bool8_t m_VSync = false;
	bool8_t m_Fullscreen = false;
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	uint32_t m_CurImageIndex = 0u;
	VkFormat m_ColorFormat = VkFormat::VK_FORMAT_UNDEFINED;

	const uint64_t m_WindowHandle;

	std::unique_ptr<VulkanSurface> m_Surface;
	std::unique_ptr<VulkanSemaphore> m_PresentComplete;
	std::vector<std::shared_ptr<class VulkanFramebuffer>> m_BackBuffers;
};

NAMESPACE_END(RHI)
