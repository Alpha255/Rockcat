#include "RHI/Vulkan/VulkanInstance.h"
#include "Runtime/Engine/Engine.h"

#if !USE_DYNAMIC_VK_LOADER
PFN_vkCreateDebugUtilsMessengerEXT PFN_CreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT PFN_DestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
	VkInstance Instance,
	const VkDebugUtilsMessengerCreateInfoEXT* CreateInfo,
	const VkAllocationCallbacks* Allocator,
	VkDebugUtilsMessengerEXT* Messenger) 
{
	return PFN_CreateDebugUtilsMessengerEXT(Instance, CreateInfo, Allocator, Messenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
	VkInstance Instance, 
	VkDebugUtilsMessengerEXT Messenger,
	VkAllocationCallbacks const* Allocator) 
{
	return PFN_DestroyDebugUtilsMessengerEXT(Instance, Messenger, Allocator);
}

PFN_vkCreateDebugReportCallbackEXT PFN_CreateDebugReportCallbackEXT;
PFN_vkDestroyDebugReportCallbackEXT PFN_DestroyDebugReportCallbackEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(
	VkInstance Instance, 
	const VkDebugReportCallbackCreateInfoEXT* CreateInfo, 
	const VkAllocationCallbacks* Allocator, 
	VkDebugReportCallbackEXT* Callback)
{
	return PFN_CreateDebugReportCallbackEXT(Instance, CreateInfo, Allocator, Callback);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(VkInstance Instance, VkDebugReportCallbackEXT Callback, const VkAllocationCallbacks* Allocator)
{
	return PFN_DestroyDebugReportCallbackEXT(Instance, Callback, Allocator);
}
#endif

static constexpr bool8_t UNIQUE_MESSAGE = true;

static VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugUtilsMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT MessageServerityFlagBits,
	VkDebugUtilsMessageTypeFlagsEXT MessageTypeFlags,
	const VkDebugUtilsMessengerCallbackDataEXT* MessengerCallbackData,
	void* UserData)
{
	(void)(UserData);
	(void)(MessageTypeFlags);

	std::string Message = StringUtils::Format("VulkanRHI: Validation: [%3d][%10s]: %s",
		MessengerCallbackData->messageIdNumber,
		MessengerCallbackData->pMessageIdName,
		MessengerCallbackData->pMessage);

	if (EnumHasAnyFlags(MessageServerityFlagBits, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT))
	{
#if 0
		LOG_INFO("VulkanRHI Validation: {:<3}{:<10}: {}", MessengerCallbackData->messageIdNumber, MessengerCallbackData->pMessageIdName, MessengerCallbackData->pMessage);
#else
		LOG_INFO("{}", Message);
#endif
	}
	else if (EnumHasAnyFlags(MessageServerityFlagBits, VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT))
	{
		LOG_WARNING("{}", Message);
	}
	else if (EnumHasAnyFlags(MessageServerityFlagBits, VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT))
	{
		LOG_ERROR("{}", Message);
	}
	else if (EnumHasAnyFlags(MessageServerityFlagBits, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT))
	{
		LOG_DEBUG("{}", Message);
	}
	else
	{
		assert(0);
	}

	return VK_FALSE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugReportCallback(
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
		LOG_ERROR("VulkanRHI: {}: {}", LayerPrefix, Message);
	}
	else if (EnumHasAnyFlags(Flags, VK_DEBUG_REPORT_WARNING_BIT_EXT) || EnumHasAnyFlags(Flags, VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT))
	{
		LOG_WARNING("VulkanRHI: {}: {}", LayerPrefix, Message);
	}
	else
	{
		LOG_INFO("VulkanRHI: {}: {}", LayerPrefix, Message);
	}

	return VK_FALSE;
}

VulkanInstance::VulkanInstance(const VulkanLayerExtensionConfigurations* Configs)
{
	auto WantedLayers = VulkanLayer::GetWantedInstanceLayers();
	auto WantedExtensions = VulkanExtension::GetWantedInstanceExtensions();

	std::vector<const char8_t*> EnabledLayers;
	std::vector<const char8_t*> EnabledExtensions;

	LOG_DEBUG("VulkanRHI: Found supported instance layers:");
	auto LayerProperties = vk::enumerateInstanceLayerProperties();
	for each (const auto& LayerProperty in LayerProperties)
	{
		auto LayerIt = std::find_if(WantedLayers.begin(), WantedLayers.end(), [&LayerProperty](const std::unique_ptr<VulkanLayer>& Layer) {
			return strcmp(Layer->GetName(), LayerProperty.layerName.data()) == 0;
			});
		if (LayerIt != WantedLayers.end())
		{
			EnabledLayers.push_back(LayerProperty.layerName.data());
			(*LayerIt)->SetEnabled(Configs, true);
		}

		LOG_DEBUG("\t\t\t\t\"{}\"", LayerProperty.layerName.data());
	}

	VulkanExtensionArray::iterator DebugUtilExt = WantedExtensions.end();
	VulkanExtensionArray::iterator DebugReportExt = WantedExtensions.end();

	LOG_DEBUG("VulkanRHI: Found supported instance extensions:");
	auto ExtensionProperties = vk::enumerateInstanceExtensionProperties();
	for each (const auto& ExtensionProperty in ExtensionProperties)
	{
		auto ExtensionIt = std::find_if(WantedExtensions.begin(), WantedExtensions.end(), [&ExtensionProperty](const std::unique_ptr<VulkanExtension>& Extension) {
			return strcmp(Extension->GetName(), ExtensionProperty.extensionName.data()) == 0;
			});
		if (ExtensionIt != WantedExtensions.end())
		{
			EnabledExtensions.push_back(ExtensionProperty.extensionName.data());
			(*ExtensionIt)->SetEnabled(Configs, true);

			if (std::strcmp((*ExtensionIt)->GetName(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
			{
				DebugUtilExt = ExtensionIt;
			}
			if (std::strcmp((*ExtensionIt)->GetName(), VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0)
			{
				DebugReportExt = ExtensionIt;
			}
		}

		LOG_DEBUG("\t\t\t\t\"{}\"", ExtensionProperty.extensionName.data());
	}

	if (DebugUtilExt != WantedExtensions.end() && (*DebugUtilExt)->IsEnabled())
	{
		if (DebugReportExt != WantedExtensions.end() && (*DebugReportExt)->IsEnabled())
		{
			(*DebugReportExt)->SetEnabled(Configs, false);
			EnabledExtensions.erase(std::find_if(EnabledExtensions.begin(), EnabledExtensions.end(), [](const char8_t* ExtensionName) {
				return strcmp(ExtensionName, VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0;
				}));
		}
	}

	LogEnableLayerAndExtensions(WantedLayers, WantedExtensions, "instance");

	auto ApplicationInfo = vk::ApplicationInfo()
		.setApplicationVersion(VK_HEADER_VERSION_COMPLETE);

	auto CreateInfo = vk::InstanceCreateInfo()
		.setPEnabledLayerNames(EnabledLayers)
		.setPEnabledExtensionNames(EnabledExtensions)
		.setPApplicationInfo(&ApplicationInfo);

	for each (auto& Extension in WantedExtensions)
	{
		if (Extension->IsEnabled())
		{
			Cast<VulkanInstanceExtension>(Extension)->PreInstanceCreation(Configs, CreateInfo);
		}
	}

	VERIFY_VK(vk::createInstance(&CreateInfo, nullptr, &m_Instance));

#if USE_DYNAMIC_VK_LOADER
	VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance);
#endif

	SetupRuntimeDebug(Configs, (*DebugUtilExt)->IsEnabled(), (*DebugReportExt)->IsEnabled());
}

void VulkanInstance::SetupRuntimeDebug(const VulkanLayerExtensionConfigurations* Configs, bool8_t EnableDebugUtils, bool8_t EnableDebugReports)
{
	if (Configs->DebugLayerLevel > ERHIDebugLayerLevel::None)
	{
		if (EnableDebugUtils)
		{
#if !USE_DYNAMIC_VK_LOADER
			PFN_CreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(m_Instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
			PFN_DestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(m_Instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
			assert(PFN_CreateDebugUtilsMessengerEXT && PFN_DestroyDebugUtilsMessengerEXT);
#endif

			vk::DebugUtilsMessageSeverityFlagsEXT MessageSeverityFlags;
			vk::DebugUtilsMessageTypeFlagsEXT MessageTypeFlags;
			switch (Configs->DebugLayerLevel)
			{
			case ERHIDebugLayerLevel::Verbose:
				MessageSeverityFlags |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;
				MessageTypeFlags |= vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding;
			case ERHIDebugLayerLevel::Error:
				MessageSeverityFlags |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
				MessageTypeFlags |= vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
			case ERHIDebugLayerLevel::Warning:
				MessageSeverityFlags |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
				MessageTypeFlags |= vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
			case ERHIDebugLayerLevel::Info:
				MessageSeverityFlags |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
				MessageTypeFlags |= vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral;
				break;
			}

			auto CreateInfo = vk::DebugUtilsMessengerCreateInfoEXT()
				.setMessageSeverity(MessageSeverityFlags)
				.setMessageType(MessageTypeFlags)
				.setPfnUserCallback(vkDebugUtilsMessengerCallback)
				.setPUserData(this);
			VERIFY_VK(m_Instance.createDebugUtilsMessengerEXT(&CreateInfo, nullptr, &m_DebugUtilsMessenger));
		}
		else if (EnableDebugReports)
		{
#if !USE_DYNAMIC_VK_LOADER
			PFN_CreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(m_Instance.getProcAddr("vkCreateDebugReportCallbackEXT"));
			PFN_DestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(m_Instance.getProcAddr("DestroyDebugReportCallbackEXT"));
			assert(PFN_CreateDebugReportCallbackEXT && PFN_DestroyDebugReportCallbackEXT);
#endif

			vk::DebugReportFlagsEXT DebugReportFlags;
			switch (Configs->DebugLayerLevel)
			{
			case ERHIDebugLayerLevel::Verbose:
				DebugReportFlags |= vk::DebugReportFlagBitsEXT::eDebug;
			case ERHIDebugLayerLevel::Error:
				DebugReportFlags |= vk::DebugReportFlagBitsEXT::eError;
			case ERHIDebugLayerLevel::Warning:
				DebugReportFlags |= vk::DebugReportFlagBitsEXT::eWarning;
				DebugReportFlags |= vk::DebugReportFlagBitsEXT::ePerformanceWarning;
			case ERHIDebugLayerLevel::Info:
				DebugReportFlags |= vk::DebugReportFlagBitsEXT::eInformation;
				break;
			}

			auto CreateInfo = vk::DebugReportCallbackCreateInfoEXT()
				.setFlags(DebugReportFlags)
				.setPfnCallback(vkDebugReportCallback)
				.setPUserData(this);
			VERIFY_VK(m_Instance.createDebugReportCallbackEXT(&CreateInfo, nullptr, &m_DebugReportCallback));
		}
	}
}

VulkanInstance::~VulkanInstance()
{
	if (m_DebugUtilsMessenger)
	{
		m_Instance.destroy(m_DebugUtilsMessenger);
		m_DebugUtilsMessenger = nullptr;
	}

	if (m_DebugReportCallback)
	{
		m_Instance.destroy(m_DebugReportCallback);
		m_DebugReportCallback = nullptr;
	}

	m_Instance.destroy();
	m_Instance = nullptr;
}