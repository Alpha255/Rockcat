#include "RHI/Vulkan/VulkanRHI.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanSwapchain.h"
#include "RHI/Vulkan/VulkanLayerExtensions.h"
#include "RHI/Vulkan/VulkanEnvConfiguration.h"
#include "Window.h"

#if USE_DYNAMIC_VK_LOADER
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

std::shared_ptr<VulkanEnvConfiguration> VulkanRHI::s_EnvConfigs;

VulkanRHI::VulkanRHI()
{
	REGISTER_LOG_CATEGORY(LogVulkanRHI);
}

void VulkanRHI::Initialize()
{
#if USE_DYNAMIC_VK_LOADER
	const PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_DynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	assert(vkGetInstanceProcAddr);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);	
#endif
	s_EnvConfigs = VulkanEnvConfiguration::Load(VK_ENV_CONFIG_PATH);
	m_Device = std::make_unique<VulkanDevice>(s_EnvConfigs->ExtensionConfigs);
	s_EnvConfigs->Save(true);
}

VulkanRHI::~VulkanRHI()
{
	m_Device.reset();

	LOG_CAT_INFO(LogVulkanRHI, "Finalized");
}

RHIDevice& VulkanRHI::GetDevice()
{
	return *m_Device;
}

const VulkanExtensionConfiguration& VulkanRHI::GetExtConfigs()
{
	return s_EnvConfigs->ExtensionConfigs;
}

const VulkanDescriptorLimitationConfiguration& VulkanRHI::GetDescriptorLimitationConfigs()
{
	return s_EnvConfigs->DescriptorLimitationConfigs;
}

#if defined(DYNAMIC_LIBRARY)
extern "C"
{
	EXPORT_API void CreateRenderer(IRendererPtr& RendererPtr, const RenderSettings* Settings)
	{
		RendererPtr = std::make_unique<VulkanRenderer>(Settings);
	}
}
#endif
