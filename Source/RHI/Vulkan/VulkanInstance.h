#pragma once

#include "Colorful/Vulkan/VulkanTypes.h"

NAMESPACE_START(RHI)

template <typename T>
std::vector<const char8_t*> GetSupportedProperties(
	const std::vector<T>& SupportedProperties, 
	const std::vector<const char8_t*>& TargetProperties)
{
	std::vector<const char8_t*> Ret;

	for each (auto PropertyName in TargetProperties)
	{
		for (auto It = SupportedProperties.begin(); It != SupportedProperties.end(); ++It)
		{
			if (_stricmp((const char8_t* const)(&(*It)), PropertyName) == 0)
			{
				Ret.push_back(PropertyName);
				break;
			}
		}
	}

	return Ret;
}

std::string GetEnabledPropertiesMessage(const std::vector<const char8_t*>& Properties);

class VulkanInstance final : public VkHWObject<void, VkInstance_T>
{
public:
	VulkanInstance(bool8_t Verbose);

	~VulkanInstance();

	const bool8_t SupportDebugUtils() const
	{
		return m_DebugUtils != VK_NULL_HANDLE;
	}

	const bool8_t SupportDebugReport() const
	{
		return m_DebugReport != VK_NULL_HANDLE;
	}
private:
#if VK_EXT_debug_utils
	VkDebugUtilsMessengerEXT m_DebugUtils = VK_NULL_HANDLE;
#endif
#if VK_EXT_debug_report
	VkDebugReportCallbackEXT m_DebugReport = VK_NULL_HANDLE;
#endif
};

NAMESPACE_END(RHI)
