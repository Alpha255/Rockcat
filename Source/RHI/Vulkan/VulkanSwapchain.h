#pragma once

#include "RHI/Vulkan/VulkanAsync.h"
#include "RHI/Vulkan/VulkanTexture.h"
#include "RHI/RHISwapchain.h"

class VulkanSurface final : public VkDeviceResource<vk::SurfaceKHR>
{
public:
	VulkanSurface(const class VulkanDevice& Device, const void* WindowHandle);

	~VulkanSurface();
};

class VulkanSwapchain final : public VkHwResource<vk::SwapchainKHR>, public RHISwapchain
{
public:
	VulkanSwapchain(const class VulkanDevice& Device, const RHISwapchainCreateInfo& CreateInfo);

	void Resize(uint32_t Width, uint32_t Height) override final;

#if 0
	VkSemaphore PresentCompleteSemaphore() const
	{
		return m_PresentComplete->Get();
	}
#endif

	void AdvanceFrame() override final;
	void Present() override final;

	inline vk::Format GetColorFormat() const { return m_ColorFormat; }
protected:
	void Create(bool RecreateSurface);
	void Recreate();
private:
	vk::Format m_ColorFormat = vk::Format::eUndefined;
	std::unique_ptr<VulkanSurface> m_Surface;
	//std::unique_ptr<VulkanSemaphore> m_PresentComplete;
};
