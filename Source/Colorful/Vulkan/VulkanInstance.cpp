#include "Colorful/Vulkan/VulkanInstance.h"
#include "Colorful/Vulkan/VulkanDevice.h"

NAMESPACE_START(RHI)

std::string GetEnabledPropertiesMessage(const std::vector<const char8_t*>& Properties)
{
	std::string Message;
	for (auto Property : Properties)
	{
		Message += "\n\t\t\t";
		Message += Property;
	}
	return Message;
}

#if VK_EXT_debug_utils
VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT MessageServerityFlagBits,
	VkDebugUtilsMessageTypeFlagsEXT MessageTypeFlags,
	const VkDebugUtilsMessengerCallbackDataEXT* MessengerCallbackData,
	void* UserData)
{
	(void)(UserData);
	(void)(MessageTypeFlags);

	std::string Message(std::move(Gear::String::Format("Vulkan Validation: [%3d][%10s]: %s",
		MessengerCallbackData->messageIdNumber,
		MessengerCallbackData->pMessageIdName,
		MessengerCallbackData->pMessage)));

	if (MessageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	{
		LOG_INFO("{}", Message);
	}
	else if (MessageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		LOG_WARNING("{}", Message);
	}
	else if (MessageServerityFlagBits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		LOG_ERROR("{}", Message);
	}

	return VK_FALSE;
}
#endif

#if VK_EXT_debug_report
VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
	VkDebugReportFlagsEXT Flags, 
	VkDebugReportObjectTypeEXT Type, 
	uint64_t Handle, 
	size_t Location, 
	int32_t MessageCode, 
	const char8_t* LayerPrefix, 
	const char8_t* Message, 
	void* UserData)
{
	(void)Type;
	(void)Handle;
	(void)Location;
	(void)MessageCode;
	(void)UserData;

	if (EnumHasAnyFlags(Flags, VK_DEBUG_REPORT_ERROR_BIT_EXT))
	{
		LOG_ERROR("{}: {}", LayerPrefix, Message);
	}
	else if (EnumHasAnyFlags(Flags, VK_DEBUG_REPORT_WARNING_BIT_EXT) || EnumHasAnyFlags(Flags, VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT))
	{
		LOG_WARNING("{}: {}", LayerPrefix, Message);
	}
	else
	{
		LOG_INFO("{}: {}", LayerPrefix, Message);
	}

	return VK_FALSE;
}
#endif

VulkanInstance::VulkanInstance(bool8_t Verbose)
{
	std::vector<const char8_t*> EnabledLayers
	{
#if defined(_DEBUG)
		"VK_LAYER_KHRONOS_validation"
#endif
	};

	std::vector<const char8_t*> EnabledExtensions
	{
#if VK_KHR_surface
		VK_KHR_SURFACE_EXTENSION_NAME,
#endif
#if defined(PLATFORM_WIN32)
#if VK_KHR_win32_surface
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
#elif defined(PLATFORM_LINUX)
		VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
	};

	uint32_t Count = 0U;
	VERIFY_VK(vkEnumerateInstanceLayerProperties(&Count, nullptr));
	std::vector<VkLayerProperties> SupportedLayers(Count);
	VERIFY_VK(vkEnumerateInstanceLayerProperties(&Count, SupportedLayers.data()));
	EnabledLayers = GetSupportedProperties<VkLayerProperties>(SupportedLayers, EnabledLayers);
	LOG_INFO("Enabled VkInstance layers: {}", GetEnabledPropertiesMessage(EnabledLayers));

	VERIFY_VK(vkEnumerateInstanceExtensionProperties(nullptr, &Count, nullptr));
	std::vector<VkExtensionProperties> SupportedExtensions(Count);
	VERIFY_VK(vkEnumerateInstanceExtensionProperties(nullptr, &Count, SupportedExtensions.data()));

	auto SupportDebugUtils = false;
	auto SupportDebugReport = false;

#if defined(_DEBUG)
#if VK_EXT_debug_utils
	SupportDebugUtils = std::find_if(SupportedExtensions.begin(), SupportedExtensions.end(), [](const VkExtensionProperties& ExtProperty) {
		return std::strcmp(ExtProperty.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0;
		}) != SupportedExtensions.end();
		
	if (SupportDebugUtils)
	{
		EnabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
#endif

#if VK_EXT_debug_report
	if (!SupportDebugUtils)
	{
		SupportDebugReport = std::find_if(EnabledExtensions.begin(), EnabledExtensions.end(), [](auto Extension) {
			return std::strcmp(Extension, VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0;
		}) != EnabledExtensions.end();

		if (SupportDebugReport)
		{
			EnabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}
	}
#endif
#endif

	EnabledExtensions = GetSupportedProperties<VkExtensionProperties>(SupportedExtensions, EnabledExtensions);
	LOG_INFO("Enabled VkInstance extensions: {}", GetEnabledPropertiesMessage(EnabledExtensions));

	if (std::find_if(EnabledExtensions.begin(), EnabledExtensions.end(), [](auto Extension) {
			return std::strcmp(Extension, VK_KHR_SURFACE_EXTENSION_NAME) == 0;
		}) == EnabledExtensions.end())
	{
		LOG_ERROR("The surface extension is not supported on this device.");
		assert(false);
	}

#if defined(PLATFORM_WIN32)
	if (std::find_if(EnabledExtensions.begin(), EnabledExtensions.end(), [](auto Extension) {
			return std::strcmp(Extension, VK_KHR_WIN32_SURFACE_EXTENSION_NAME) == 0;
		}) == EnabledExtensions.end())
	{
		LOG_ERROR("The Win32 surface extension is not supported on this device.");
		assert(false);
	}
#endif

	VkApplicationInfo ApplicationInfo
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"VkApplication",
		0u,
		"VkRenderer",
		0u,
		VK_HEADER_VERSION_COMPLETE
	};

	VkInstanceCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0u,
		&ApplicationInfo,
		static_cast<uint32_t>(EnabledLayers.size()),
		EnabledLayers.data(),
		static_cast<uint32_t>(EnabledExtensions.size()),
		EnabledExtensions.data()
	};

#if defined(_DEBUG)
#if VK_EXT_validation_features
#if defined(VK_ENABLE_VALIDATION_FEATURES) /// #TODO ???
	VkValidationFeatureEnableEXT ValidationFeaturesEnabled[]
	{
		VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
		VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT
	};
	VkValidationFeaturesEXT ValidationFeatures
	{
		VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
		nullptr,
		_countof(ValidationFeaturesEnabled),
		ValidationFeaturesEnabled,
		0u,
		nullptr
	};

	CreateInfo.pNext = &ValidationFeatures;
#endif
#endif
#endif

	VERIFY_VK(vkCreateInstance(&CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));

#if VK_EXT_debug_utils
	if (SupportDebugUtils)
	{
		VkDebugUtilsMessageSeverityFlagsEXT MessageServityFlags = Verbose ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT :
			(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);

		VkDebugUtilsMessengerCreateInfoEXT DebugUtilsCreateInfo
		{
			VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			nullptr,
			0u,
			MessageServityFlags,
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			DebugUtilsMessengerCallback,
			nullptr
		};
		PFN_vkCreateDebugUtilsMessengerEXT VkCreateDebugUtilsMesserger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(m_Handle, "vkCreateDebugUtilsMessengerEXT"));
		assert(VkCreateDebugUtilsMesserger);

		VERIFY_VK(VkCreateDebugUtilsMesserger(m_Handle, &DebugUtilsCreateInfo, VK_ALLOCATION_CALLBACKS, &m_DebugUtils));
	}
#endif

#if VK_EXT_debug_report
	if (SupportDebugReport)
	{
		VkDebugReportCallbackCreateInfoEXT DebugReportCreateInfo
		{
			VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,
			nullptr,
			VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
			DebugReportCallback,
			nullptr
		};

		PFN_vkCreateDebugReportCallbackEXT VkCreateDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
			vkGetInstanceProcAddr(m_Handle, "vkCreateDebugReportCallbackEXT"));
		assert(VkCreateDebugReportCallback);

		VERIFY_VK(VkCreateDebugReportCallback(m_Handle, &DebugReportCreateInfo, VK_ALLOCATION_CALLBACKS, &m_DebugReport));
	}
#endif
}

VulkanInstance::~VulkanInstance()
{
#if VK_EXT_debug_utils
	if (m_DebugUtils != VK_NULL_HANDLE)
	{
		PFN_vkDestroyDebugUtilsMessengerEXT VkDestroyDebugUtilsMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(m_Handle, "vkDestroyDebugUtilsMessengerEXT"));
		assert(VkDestroyDebugUtilsMessenger);
		VkDestroyDebugUtilsMessenger(m_Handle, m_DebugUtils, VK_ALLOCATION_CALLBACKS);
		m_DebugUtils = VK_NULL_HANDLE;
	}
#endif

#if VK_EXT_debug_report
	if (m_DebugReport != VK_NULL_HANDLE)
	{
		PFN_vkDestroyDebugReportCallbackEXT VkDestroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
			vkGetInstanceProcAddr(m_Handle, "vkDestroyDebugReportCallbackEXT"));
		assert(VkDestroyDebugReportCallback);
		VkDestroyDebugReportCallback(m_Handle, m_DebugReport, VK_ALLOCATION_CALLBACKS);
		m_DebugReport = VK_NULL_HANDLE;
	}
#endif

	vkDestroyInstance(Get(), VK_ALLOCATION_CALLBACKS);
}

NAMESPACE_END(RHI)