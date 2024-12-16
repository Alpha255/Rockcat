#include "RHI/Vulkan/VulkanInstance.h"
#include "RHI/Vulkan/VulkanLayerExtensions.h"
#include "Engine/Services/SpdLogService.h"

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

static VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugUtilsMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT MessageServerityFlagBits,
	VkDebugUtilsMessageTypeFlagsEXT MessageTypeFlags,
	const VkDebugUtilsMessengerCallbackDataEXT* MessengerCallbackData,
	void* UserData)
{
	(void)(UserData);
	(void)(MessageTypeFlags);

	std::string Message = StringUtils::Format("[Validation]: [%3d][%10s]: %s",
		MessengerCallbackData->messageIdNumber,
		MessengerCallbackData->pMessageIdName,
		MessengerCallbackData->pMessage);

	if (EnumHasAnyFlags(MessageServerityFlagBits, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT))
	{
#if 0
		LOG_INFO("VulkanRHI Validation: {:<3}{:<10}: {}", MessengerCallbackData->messageIdNumber, MessengerCallbackData->pMessageIdName, MessengerCallbackData->pMessage);
#else
		LOG_CAT_INFO(LogVulkanRHI, Message.c_str());
#endif
	}
	else if (EnumHasAnyFlags(MessageServerityFlagBits, VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT))
	{
		LOG_CAT_WARNING(LogVulkanRHI, Message.c_str());
	}
	else if (EnumHasAnyFlags(MessageServerityFlagBits, VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT))
	{
		LOG_CAT_ERROR(LogVulkanRHI, Message.c_str());
	}
	else if (EnumHasAnyFlags(MessageServerityFlagBits, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT))
	{
		LOG_CAT_DEBUG(LogVulkanRHI, Message.c_str());
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
	const char* LayerPrefix,
	const char* Message,
	void* UserData)
{
	(void)Type;
	(void)Handle;
	(void)Location;
	(void)MessageCode;
	(void)UserData;

	if (EnumHasAnyFlags(Flags, VK_DEBUG_REPORT_ERROR_BIT_EXT))
	{
		LOG_CAT_ERROR(LogVulkanRHI, "{}: {}", LayerPrefix, Message);
	}
	else if (EnumHasAnyFlags(Flags, VK_DEBUG_REPORT_WARNING_BIT_EXT) || EnumHasAnyFlags(Flags, VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT))
	{
		LOG_CAT_WARNING(LogVulkanRHI, "{}: {}", LayerPrefix, Message);
	}
	else
	{
		LOG_CAT_INFO(LogVulkanRHI, "{}: {}", LayerPrefix, Message);
	}

	return VK_FALSE;
}

VulkanInstance::VulkanInstance(VulkanLayerExtensionConfigurations* Configs)
{
	auto WantedLayers = VulkanLayer::GetWantedInstanceLayers(*Configs);
	auto WantedExtensions = VulkanExtension::GetWantedInstanceExtensions(*Configs);

	std::vector<const char*> EnabledLayers;
	std::vector<const char*> EnabledExtensions;

	auto LayerProperties = vk::enumerateInstanceLayerProperties();
	std::string LogValidInstanceLayers("VulkanRHI: Found valid instance layers:\n");

	for (const auto& LayerProperty : LayerProperties)
	{
		LogValidInstanceLayers += StringUtils::Format("\t\t\t\t\"%s\"\n", LayerProperty.layerName.data());
	}
	LOG_CAT_DEBUG(LogVulkanRHI, LogValidInstanceLayers.c_str());
	
	for (auto& Layer : WantedLayers)
	{
		auto LayerPropertyIt = std::find_if(LayerProperties.cbegin(), LayerProperties.cend(), [&Layer](const vk::LayerProperties& Property) {
			return strcmp(Property.layerName.data(), Layer->GetName());
		});
		
		Layer->SetEnabledInConfig(LayerPropertyIt != LayerProperties.cend());
		
		if (Layer->IsEnabled())
		{
			EnabledLayers.push_back(Layer->GetName());
		}
	}

	VulkanExtension* DebugUtilExt = nullptr;
	VulkanExtension* DebugReportExt = nullptr;

	std::string LogValidInstanceExtensions("Found valid instance extensions:\n");
	auto ExtensionProperties = vk::enumerateInstanceExtensionProperties();

	for (const auto& ExtensionProperty : ExtensionProperties)
	{
		LogValidInstanceExtensions += StringUtils::Format("\t\t\t\t\"%s\"\n", ExtensionProperty.extensionName.data());
	}
	LOG_CAT_DEBUG(LogVulkanRHI, LogValidInstanceExtensions.c_str());

	for (auto& Ext : WantedExtensions)
	{
		auto ExtensionIt = std::find_if(ExtensionProperties.cbegin(), ExtensionProperties.cend(), [&Ext](const vk::ExtensionProperties& Property) {
			return strcmp(Property.extensionName.data(), Ext->GetName()) == 0;
		});

		Ext->SetEnabledInConfig(ExtensionIt != ExtensionProperties.cend());

		if (Ext->IsEnabled())
		{
			EnabledExtensions.push_back(Ext->GetName());

			if (std::strcmp(Ext->GetName(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
			{
				DebugUtilExt = Ext.get();
			}
			if (std::strcmp(Ext->GetName(), VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0)
			{
				DebugReportExt = Ext.get();
			}
		}
	}

	if (DebugUtilExt && DebugUtilExt->IsEnabled())
	{
		if (DebugReportExt && DebugReportExt->IsEnabled())
		{
			DebugReportExt->SetEnabledInConfig(false);
			EnabledExtensions.erase(std::find_if(EnabledExtensions.begin(), EnabledExtensions.end(), [](const char* ExtensionName) {
				return strcmp(ExtensionName, VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0;
			}));
		}
	}

	LogEnabledLayerAndExtensions(WantedLayers, WantedExtensions, "instance");

	vk::ApplicationInfo ApplicationInfo;
	ApplicationInfo.setApplicationVersion(VK_HEADER_VERSION_COMPLETE);

	vk::InstanceCreateInfo CreateInfo;
	CreateInfo.setPEnabledLayerNames(EnabledLayers)
		.setPEnabledExtensionNames(EnabledExtensions)
		.setPApplicationInfo(&ApplicationInfo);

	for (auto& Extension : WantedExtensions)
	{
		if (Extension->IsEnabled() && Extension->GetOnInstanceCreation())
		{
			Extension->GetOnInstanceCreation()(*Configs, CreateInfo);
		}
	}

	VERIFY_VK(vk::createInstance(&CreateInfo, nullptr, &m_Instance));

#if USE_DYNAMIC_VK_LOADER
	VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance);
#endif

	SetupRuntimeDebug(Configs, DebugUtilExt ? DebugUtilExt->IsEnabled() : false, DebugReportExt ? DebugReportExt->IsEnabled() : false);
}

void VulkanInstance::SetupRuntimeDebug(const VulkanLayerExtensionConfigurations* Configs, bool EnableDebugUtils, bool EnableDebugReports)
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

			vk::DebugUtilsMessengerCreateInfoEXT CreateInfo;
			CreateInfo.setMessageSeverity(MessageSeverityFlags)
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

			vk::DebugReportCallbackCreateInfoEXT CreateInfo;
			CreateInfo.setFlags(DebugReportFlags)
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