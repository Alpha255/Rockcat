#pragma once

#include "RHI/Vulkan/VulkanTypes.h"

struct VulkanLayer
{
	VulkanLayer(const char8_t* InName, bool8_t Needed = false)
		: Name(InName)
		, IsNeeded(Needed)
	{
	}

	inline bool8_t operator==(const VulkanLayer& Other) const { return Name == Other.Name; }
	inline bool8_t operator!=(const VulkanLayer& Other) const { return Name != Other.Name; }

	std::string_view Name;
	bool8_t IsNeeded = false;
};
using VulkanExtension = VulkanLayer;

class VulkanInstance final
{
public:
	VulkanInstance(ERHIDebugLayerLevel DebugLevel);

	~VulkanInstance();

	inline vk::Instance& Get() { assert(!m_Instance); return m_Instance; }
private:
	void SetupRuntimeDebug(ERHIDebugLayerLevel DebugLevel, bool8_t EnableDebugUtils, bool8_t EnableDebugReports);

	vk::Instance m_Instance;
	vk::DebugReportCallbackEXT m_DebugReportCallback;
	vk::DebugUtilsMessengerEXT m_DebugUtilsMessenger;
};
