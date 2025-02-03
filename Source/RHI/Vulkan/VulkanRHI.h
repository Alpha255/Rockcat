#pragma once

#include "Engine/RHI/RHIInterface.h"
#include "Engine/RHI/RHIFormat.h"

class VulkanRHI final : public RHIInterface
{
public:
	VulkanRHI(const GraphicsSettings* GfxSettings);

	~VulkanRHI();

	ERHIBackend GetRHIType() const override final { return ERHIBackend::Vulkan; }

	RHIDevice& GetDevice() override final;

	static const GraphicsSettings& GetGraphicsSettings() { return RHIInterface::GetGraphicsSettings(ERHIBackend::Vulkan); }

	static const struct VulkanLayerExtensionConfigurations& GetLayerExtensionConfigs() { return *s_LayerExtensionConfigs; }
protected:
	void InitializeGraphicsDevices() override final;
private:
	std::unique_ptr<class VulkanDevice> m_Device;

	static std::shared_ptr<struct VulkanLayerExtensionConfigurations> s_LayerExtensionConfigs;

#if USE_DYNAMIC_VK_LOADER
	const vk::detail::DynamicLoader m_DynamicLoader;
#endif
};
