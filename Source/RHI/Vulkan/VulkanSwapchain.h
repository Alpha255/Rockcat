#pragma once

#include "RHI/Vulkan/VulkanAsync.h"
#include "RHI/Vulkan/VulkanTexture.h"

class VulkanSurface final : public VkDeviceResource<vk::SurfaceKHR>
{
public:
	VulkanSurface(const class VulkanDevice& Device, const void* WindowHandle);

	~VulkanSurface();
};

class VulkanSwapchain final : public VkHwResource<vk::SwapchainKHR>
{
public:
	VulkanSwapchain(const class VulkanDevice& Device, const void* WindowHandle, uint32_t Width, uint32_t Height, bool VSync, bool Fullscreen, bool HDR);

	void Resize(uint32_t Width, uint32_t Height)
	{
		if ((Width && Height) && (m_Width != Width || m_Height != Height))
		{
			m_Width = Width;
			m_Height = Height;
		}
	}

	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() const { return m_Height; }
	inline uint32_t GetCurrentImageIndex() const { return m_CurImageIndex; }
	inline vk::Format GetColorFormat() const { return m_ColorFormat; }

#if 0
	VkSemaphore PresentCompleteSemaphore() const
	{
		return m_PresentComplete->Get();
	}

	class VulkanFramebuffer* CurrentFrameBuffer() const
	{
		return m_BackBuffers[m_CurImageIndex].get();
	}
#endif

	void AcquireNextImage();

	void Present();
protected:
	void Create(bool RecreateSurface);

	void Recreate();
private:
	bool m_VSync = false;
	bool m_Fullscreen = false;
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	uint32_t m_CurImageIndex = 0u;
	vk::Format m_ColorFormat = vk::Format::eUndefined;

	const void* m_WindowHandle = nullptr;

	std::unique_ptr<VulkanSurface> m_Surface;
	//std::unique_ptr<VulkanSemaphore> m_PresentComplete;
	//std::vector<std::shared_ptr<class VulkanFramebuffer>> m_BackBuffers;
};
