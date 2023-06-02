#pragma once

#include "RHI/Vulkan/VulkanLayerExtensions.h"

class VulkanInstance final
{
public:
	VulkanInstance(const VulkanLayerExtensionConfigurations* Configs);

	~VulkanInstance();

	inline vk::Instance& Get() { assert(m_Instance); return m_Instance; }
private:
	void SetupRuntimeDebug(const VulkanLayerExtensionConfigurations* Configs, bool8_t EnableDebugUtils, bool8_t EnableDebugReports);

	vk::Instance m_Instance;
	vk::DebugReportCallbackEXT m_DebugReportCallback;
	vk::DebugUtilsMessengerEXT m_DebugUtilsMessenger;
};
