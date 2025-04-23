#pragma once

#include "RHI/RHIBackend.h"
#include "RHI/RHIFormat.h"

class VulkanRHI final : public RHIBackend
{
public:
	VulkanRHI();
	~VulkanRHI();

	ERHIBackend GetType() const override final { return ERHIBackend::Vulkan; }
	RHIDevice& GetDevice() override final;

	void AdvanceFrame() override final;
	RHITexture* GetBackBuffer() override final;

	static const struct VulkanEnvConfiguration& GetEnvConfigs() { return *s_EnvConfigs; }
	static const struct VulkanExtensionConfiguration& GetExtConfigs();
	static const struct VulkanDescriptorLimitationConfiguration& GetDescriptorLimitationConfigs();
protected:
	void Initialize(const class Window& RenderWindow, const RenderSettings& GraphicsSettings) override final;
private:
	std::unique_ptr<class VulkanDevice> m_Device;
	std::unique_ptr<class VulkanSwapchain> m_Swapchain;
	static std::shared_ptr<struct VulkanEnvConfiguration> s_EnvConfigs;

#if USE_DYNAMIC_VK_LOADER
	const vk::detail::DynamicLoader m_DynamicLoader;
#endif
};
