#pragma once

#include "Colorful/Vulkan/VulkanTypes.h"

NAMESPACE_START(RHI)

class VulkanRenderer final : public IRenderer
{
public:
	VulkanRenderer(const RenderSettings* Settings, uint64_t WindowHandle);

	~VulkanRenderer();

	IDevice* Device() override final;

	void AdvanceNextFrame() override final;

	void Present() override final;

	void OnWindowResized(uint32_t Width, uint32_t Height) override final;

	IFrameBuffer* SwapchainFrameBuffer() override final;

	ERenderer Type() const override final
	{
		return ERenderer::Vulkan;
	}

	const char8_t* Name() const override final
	{
		return "Vulkan";
	}
protected:
private:
	std::unique_ptr<class VulkanInstance> m_Instance = nullptr;
	std::unique_ptr<class VulkanDevice> m_Device = nullptr;
	std::unique_ptr<class VulkanSwapchain> m_Swapchain = nullptr;
};

NAMESPACE_END(RHI)
