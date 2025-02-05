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

	static const struct VulkanEnvConfiguration& GetEnvConfigs() { return *s_EnvConfigs; }
	static const struct VulkanExtensionConfiguration& GetExtConfigs();
	static const struct VulkanDescriptorLimitationConfiguration& GetDescriptorLimitationConfigs();
	static const GraphicsSettings& GetGraphicsSettings() { return RHIInterface::GetGraphicsSettings(ERHIBackend::Vulkan); }
protected:
	void InitializeGraphicsDevices() override final;
private:
	std::unique_ptr<class VulkanDevice> m_Device;
	static std::shared_ptr<struct VulkanEnvConfiguration> s_EnvConfigs;

#if USE_DYNAMIC_VK_LOADER
	const vk::detail::DynamicLoader m_DynamicLoader;
#endif
};
