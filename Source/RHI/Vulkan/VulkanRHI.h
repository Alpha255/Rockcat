#pragma once

#include "Runtime/Engine/RHI/RHIInterface.h"
#include "Runtime/Engine/RHI/RHIFormat.h"

class VulkanRHI final : public RHIInterface
{
public:
	VulkanRHI(const GraphicsSettings* GfxSettings);

	~VulkanRHI();

	ERenderHardwareInterface GetRHIType() const override final { return ERenderHardwareInterface::Vulkan; }

	static const struct VulkanLayerExtensionConfigurations& GetLayerExtensionConfigs() { return *s_LayerExtensionConfigs; }
protected:
	void InitializeGraphicsDevices() override final;
private:
	std::unique_ptr<class VulkanDevice> m_Device;

	static std::shared_ptr<struct VulkanLayerExtensionConfigurations> s_LayerExtensionConfigs;

#if USE_DYNAMIC_VK_LOADER
	const vk::DynamicLoader m_DynamicLoader;
#endif
};
