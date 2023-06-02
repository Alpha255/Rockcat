#include "RHI/Vulkan/VulkanInstance.h"
#include "Runtime/Engine/Engine.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

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
	}

	VulkanExtensionArray::iterator DebugUtilExt = WantedExtensions.end();
	VulkanExtensionArray::iterator DebugReportExt = WantedExtensions.end();

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
			Extension->PreInstanceCreation(Configs, CreateInfo);
		}
	}

	VERIFY_VK(vk::createInstance(&CreateInfo, nullptr, &m_Instance));

	VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance);

	SetupRuntimeDebug(Configs, (*DebugUtilExt)->IsEnabled(), (*DebugReportExt)->IsEnabled());
}

void VulkanInstance::SetupRuntimeDebug(const VulkanLayerExtensionConfigurations* Configs, bool8_t EnableDebugUtils, bool8_t EnableDebugReports)
{
	if (Configs->DebugLayerLevel > ERHIDebugLayerLevel::None)
	{
		if (EnableDebugUtils)
		{
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