#pragma once

#include "Colorful/IRenderer/IRenderer.h"

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.hpp>

NAMESPACE_START(RHI)

#define VERIFY_VK(Func) assert((Func) == vk::Result::eSuccess)

class VulkanInstance final
{
public:
	VulkanInstance(bool8_t EnableRuntimeDebug = true)
	{
		const vk::DynamicLoader DllLoader;
		VULKAN_HPP_DEFAULT_DISPATCHER.init(DllLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));

		std::vector<std::string> TryToEnableLayers
		{
	#if defined(_DEBUG)
			"VK_LAYER_KHRONOS_validation"
	#endif
		};
		std::vector<const char8_t*> EnabledLayers;

		std::vector<std::string> TryToEnableExtensions
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
		std::vector<const char8_t*> EnabledExtensions;

		for (const auto& LayerProperty : vk::enumerateInstanceLayerProperties())
		{
			if (std::find(TryToEnableLayers.begin(), TryToEnableLayers.end(), std::string(LayerProperty.layerName)) != TryToEnableLayers.end())
			{
				EnabledLayers.push_back(LayerProperty.layerName.data());
			}
		}

		bool8_t SupportDebugReport = false, SupportDebugUtils = false;
		for (const auto& ExtensionProperty : vk::enumerateInstanceExtensionProperties())
		{
			if (std::find(TryToEnableExtensions.begin(), TryToEnableExtensions.end(), std::string(ExtensionProperty.extensionName)) != TryToEnableExtensions.end())
			{
				EnabledExtensions.push_back(ExtensionProperty.extensionName.data());
			}

			if (EnableRuntimeDebug)
			{
				if (!SupportDebugUtils)
				{
					SupportDebugUtils = std::strcmp(ExtensionProperty.extensionName.data(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0;
				}
				if (!SupportDebugReport)
				{
					SupportDebugReport = std::strcmp(ExtensionProperty.extensionName.data(), VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0;
				}
			}
		}

		if (EnableRuntimeDebug)
		{
			if (SupportDebugUtils)
			{
				EnabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			}
			else if (SupportDebugReport)
			{
				EnabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			}
		}

		auto ApplicationInfo = vk::ApplicationInfo()
			.setApplicationVersion(VK_HEADER_VERSION_COMPLETE);

		auto CreateInfo = vk::InstanceCreateInfo()
			.setEnabledLayerCount(static_cast<uint32_t>(EnabledLayers.size()))
			.setPpEnabledLayerNames(EnabledLayers.data())
			.setEnabledExtensionCount(static_cast<uint32_t>(EnabledExtensions.size()))
			.setPpEnabledExtensionNames(EnabledExtensions.data())
			.setPApplicationInfo(&ApplicationInfo);

		VERIFY_VK(vk::createInstance(&CreateInfo, nullptr, &m_Instance));

		VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance);

		SetupRuntimeDebug(SupportDebugUtils, SupportDebugReport);
	}

	void SetupRuntimeDebug(bool8_t EnableDebugUtils, bool8_t EnableDebugReports)
	{
		if (EnableDebugUtils)
		{
			auto CreateInfo = vk::DebugUtilsMessengerCreateInfoEXT()
				.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
				.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
				.setPfnUserCallback(DebugUtilsMessengerCallback)
				.setPUserData(this);
			VERIFY_VK(m_Instance.createDebugUtilsMessengerEXT(&CreateInfo, nullptr, &m_DebugUtilsMessenger));
		}

		if (EnableDebugReports)
		{
			auto CreateInfo = vk::DebugReportCallbackCreateInfoEXT()
				.setFlags(vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning)
				.setPfnCallback(DebugReportCallback)
				.setPUserData(this);
			VERIFY_VK(m_Instance.createDebugReportCallbackEXT(&CreateInfo, nullptr, &m_DebugReportCallback));
		}
	}

	const bool8_t IsDebugReportEnabled() const
	{
		return bool8_t(DebugReportCallback);
	}

	const bool8_t IsDebugUtilsEnabled() const
	{
		return bool8_t(m_DebugUtilsMessenger);
	}

	~VulkanInstance()
	{
		if (m_DebugUtilsMessenger)
		{
			m_Instance.destroyDebugUtilsMessengerEXT(m_DebugUtilsMessenger);
			m_DebugUtilsMessenger = nullptr;
		}

		if (m_DebugReportCallback)
		{
			m_Instance.destroyDebugReportCallbackEXT(m_DebugReportCallback);
			m_DebugReportCallback = nullptr;
		}

		m_Instance.destroy();
		m_Instance = nullptr;
	}
protected:
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
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

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
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
private:
	vk::Instance m_Instance;
	vk::DebugReportCallbackEXT m_DebugReportCallback;
	vk::DebugUtilsMessengerEXT m_DebugUtilsMessenger;
};

NAMESPACE_END(RHI)