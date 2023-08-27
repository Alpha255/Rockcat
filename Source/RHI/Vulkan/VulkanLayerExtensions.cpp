#include "RHI/Vulkan/VulkanLayerExtensions.h"

#define BIND_WITH_CONFIG_GENERAL(ConfigName, ForceEnable) \
	bool8_t IsEnabledInConfig(const VulkanLayerExtensionConfigurations* Configs) const override final \
	{ \
		return Configs && (ForceEnable || Configs->ConfigName); \
	} \
	void SetEnabledToConfig(VulkanLayerExtensionConfigurations* Config) const override final \
	{ \
		Config->ConfigName = IsEnabled(); \
	}

#define BIND_WITH_CONFIG(ConfigName) BIND_WITH_CONFIG_GENERAL(ConfigName, false)
#define BIND_WITH_CONFIG_FORCE_ENABLE(ConfigName) BIND_WITH_CONFIG_GENERAL(ConfigName, true)

class VkKHRValidationLayer : public VulkanLayer
{
public:
	VkKHRValidationLayer()
		: VulkanLayer(VK_LAYER_KHRONOS_VALIDATION_NAME, false)
	{
	}

	BIND_WITH_CONFIG(HasKhronosValidationLayer)
};

class VkKHRSurfaceExt : public VulkanInstanceExtension
{
public:
	VkKHRSurfaceExt()
		: VulkanInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME, true)
	{
	}

	BIND_WITH_CONFIG_FORCE_ENABLE(HasKHRSurfaceExt)
};

class VkKHRPlatformSurfaceExt : public VulkanInstanceExtension
{
public:
#if defined(PLATFORM_WIN32)
	VkKHRPlatformSurfaceExt()
		: VulkanInstanceExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, true)
	{
	}
#elif defined(PLATFORM_LINUX)
	VkKHRPlatformSurfaceExt()
		: VulkanInstanceExtension(VK_KHR_XCB_SURFACE_EXTENSION_NAME, true)
	{
	}
#endif

	BIND_WITH_CONFIG_FORCE_ENABLE(HasKHRSurfaceExt)
};

class VkDebugUtilsExt : public VulkanInstanceExtension
{
public:
	VkDebugUtilsExt()
		: VulkanInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, false)
	{
	}

	BIND_WITH_CONFIG(HasDebugUtilsExt)
};

class VkDebugReportExt : public VulkanInstanceExtension
{
public:
	VkDebugReportExt()
		: VulkanInstanceExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, false)
	{
	}

	BIND_WITH_CONFIG(HasDebugReportExt)
};

class VkDebugMarkerExt : public VulkanDeviceExtension
{
public:
	VkDebugMarkerExt()
		: VulkanDeviceExtension(VK_EXT_DEBUG_MARKER_EXTENSION_NAME, false)
	{
	}

	BIND_WITH_CONFIG(HasDebugMarkerExt)
};

class VkGetPhysicalDeviceProperties2Ext : public VulkanInstanceExtension
{
public:
	VkGetPhysicalDeviceProperties2Ext()
		: VulkanInstanceExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, false)
	{
	}

	BIND_WITH_CONFIG(HasGetPhysicalDeviceProperties2)
};

class VkTimelineSemaphoreExt : public VulkanDeviceExtension
{
public:
	VkTimelineSemaphoreExt()
		: VulkanDeviceExtension(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME, false)
	{
	}

	BIND_WITH_CONFIG(HasTimelineSemaphore)
};

class VkFullscreenExclusiveExt : public VulkanDeviceExtension
{
public:
	VkFullscreenExclusiveExt()
		: VulkanDeviceExtension(VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME, false)
	{
	}

	BIND_WITH_CONFIG(HasFullscreenExclusive)
};

class VkDynamicStateExt : public VulkanDeviceExtension
{
public:
	VkDynamicStateExt()
		: VulkanDeviceExtension(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME, false)
	{
	}

	BIND_WITH_CONFIG(HasDynamicState)
};

class VkDepthStencilResolveExt : public VulkanDeviceExtension
{
public:
	VkDepthStencilResolveExt()
		: VulkanDeviceExtension(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME, false)
	{
	}
};

class VkValidationFeaturesExt : public VulkanInstanceExtension
{
public:
	VkValidationFeaturesExt()
		: VulkanInstanceExtension(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME, false)
	{
	}

	BIND_WITH_CONFIG(HasValidationFeaturesExt)

	virtual void PreInstanceCreation(VulkanLayerExtensionConfigurations* Configs, vk::InstanceCreateInfo& CreateInfo)
	{
		std::vector<vk::ValidationFeatureEnableEXT> EnabledFeatures;

#define APPEND_FEATURE(Enabled, Feature) if (Configs->Enabled) { EnabledFeatures.push_back(Feature); }
		APPEND_FEATURE(HasValidationFeaturesExt_GPUAssisted, vk::ValidationFeatureEnableEXT::eGpuAssisted);
		APPEND_FEATURE(HasValidationFeaturesExt_GPUAssistedReserveBindingSlot, vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot);
		APPEND_FEATURE(HasValidationFeaturesExt_BestPractices, vk::ValidationFeatureEnableEXT::eBestPractices);
		APPEND_FEATURE(HasValidationFeaturesExt_DebugPrintf, vk::ValidationFeatureEnableEXT::eDebugPrintf);
		APPEND_FEATURE(HasValidationFeaturesExt_Synchronization, vk::ValidationFeatureEnableEXT::eSynchronizationValidation);
#undef APPEND_FEATURE

		auto ValidationFeatures = vk::ValidationFeaturesEXT()
			.setEnabledValidationFeatures(EnabledFeatures);
		SetPNext(CreateInfo, ValidationFeatures);

		VulkanInstanceExtension::PreInstanceCreation(Configs, CreateInfo);
	}
};

#define APPEND_LAYER(Type) WantedLayers.emplace_back(std::make_unique<Type>())
#define APPEND_EXT(Type) WantedExts.emplace_back(std::make_unique<Type>())

VulkanLayerArray VulkanLayer::GetWantedInstanceLayers()
{
	VulkanLayerArray WantedLayers;

	APPEND_LAYER(VkKHRValidationLayer);

	return WantedLayers;
}

VulkanLayerArray VulkanLayer::GetWantedDeviceLayers()
{
	return VulkanLayerArray();
}

VulkanExtensionArray VulkanExtension::GetWantedInstanceExtensions()
{
	VulkanExtensionArray WantedExts;

	APPEND_EXT(VkKHRSurfaceExt);
	APPEND_EXT(VkKHRPlatformSurfaceExt);
	APPEND_EXT(VkDebugUtilsExt);
	APPEND_EXT(VkDebugReportExt);
	APPEND_EXT(VkValidationFeaturesExt);
	//APPEND_EXT(VkGetPhysicalDeviceProperties2Ext);

	return WantedExts;
}

VulkanExtensionArray VulkanExtension::GetWantedDeviceExtensions()
{
	VulkanExtensionArray WantedExts;

	APPEND_EXT(VkDebugMarkerExt);
	APPEND_EXT(VkTimelineSemaphoreExt);
	APPEND_EXT(VkFullscreenExclusiveExt);
	APPEND_EXT(VkDepthStencilResolveExt);

	return WantedExts;
}

#undef APPEND_EXT
#undef APPEND_LAYER

void LogEnabledLayerAndExtensions(const VulkanLayerArray& Layers, const VulkanExtensionArray& Extensions, const char8_t* Category)
{
	for each (const auto &Layer in Layers)
	{
		if (Layer->IsEnabled())
		{
			LOG_INFO("VulkanRHI: Enable {} layer: \"{}\"", Category, Layer->GetName());
		}
		else
		{
			if (Layer->IsNeeded())
			{
				LOG_ERROR("VulkanRHI: Request {} layer \"{}\" is not supported.", Category, Layer->GetName());
			}
		}
	}

	for each (const auto & Extension in Extensions)
	{
		if (Extension->IsEnabled())
		{
			LOG_INFO("VulkanRHI: Enable {} extension: \"{}\"", Category, Extension->GetName());
		}
		else
		{
			if (Extension->IsNeeded())
			{
				LOG_ERROR("VulkanRHI: Request {} layer \"{}\" is not supported.", Category, Extension->GetName());
			}
		}
	}
}
