#pragma once

#include "Engine/RHI/RHIBackend.h"
#include "Engine/RHI/RHIFormat.h"

class VulkanRHI final : public RHIBackend
{
public:
	VulkanRHI();
	~VulkanRHI();

	ERHIBackend GetType() const override final { return ERHIBackend::Vulkan; }
	RHIDevice& GetDevice() override final;

	static const struct VulkanEnvConfiguration& GetEnvConfigs() { return *s_EnvConfigs; }
	static const struct VulkanExtensionConfiguration& GetExtConfigs();
	static const struct VulkanDescriptorLimitationConfiguration& GetDescriptorLimitationConfigs();
protected:
	void InitializeGraphicsDevice() override final;
private:
	std::unique_ptr<class VulkanDevice> m_Device;
	static std::shared_ptr<struct VulkanEnvConfiguration> s_EnvConfigs;

#if USE_DYNAMIC_VK_LOADER
	const vk::detail::DynamicLoader m_DynamicLoader;
#endif
};
