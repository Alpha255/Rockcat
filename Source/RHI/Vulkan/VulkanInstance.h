#pragma once

#include "RHI/Vulkan/VulkanLoader.h"

class VulkanInstance final
{
public:
	VulkanInstance(struct VulkanExtensionSettings& Settings, ERHIDebugLayerLevel DebugLayerLevel);

	~VulkanInstance();

	inline const vk::Instance& GetNative() const { assert(m_Instance); return m_Instance; }
private:
	void SetupRuntimeDebug(ERHIDebugLayerLevel DebugLayerLevel, bool EnableDebugUtils, bool EnableDebugReports);

	vk::Instance m_Instance;
	vk::DebugReportCallbackEXT m_DebugReportCallback;
	vk::DebugUtilsMessengerEXT m_DebugUtilsMessenger;
};
