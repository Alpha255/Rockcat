#include "RHI/Vulkan/VulkanLayerExtensions.h"

template<class LastStruct, class NextStruct>
static void SetPNext(LastStruct& Last, NextStruct& Next)
{
	Next.pNext = (void*)Last.pNext;
	Last.pNext = (void*)&Next;
}

class VkKHRValidationLayer : public VulkanLayer
{
public:
	VkKHRValidationLayer()
		: VulkanLayer(VK_LAYER_KHRONOS_VALIDATION_NAME, false)
	{
	}

	bool8_t IsEnabledInConfig(const VulkanLayerExtensionConfigurations* Configs) const override final
	{
		return Configs && Configs->HasKhronosValidationLayer;
	}

	void SetEnabledToConfig(VulkanLayerExtensionConfigurations* Config) const override final
	{
		Config->HasKhronosValidationLayer = IsEnabled();
	}
};

class VkKHRSurfaceExt : public VulkanInstanceExtension
{
public:
	VkKHRSurfaceExt()
		: VulkanInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME, true)
	{
	}

	bool8_t IsEnabledInConfig(const VulkanLayerExtensionConfigurations* Configs) const override final
	{
		return Configs && true;
	}

	void SetEnabledToConfig(VulkanLayerExtensionConfigurations* Config) const override final
	{
		Config->HasKHRSurfaceExt = IsEnabled();
	}
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

	bool8_t IsEnabledInConfig(const VulkanLayerExtensionConfigurations* Configs) const override final
	{
		return Configs && true;
	}

	void SetEnabledToConfig(VulkanLayerExtensionConfigurations* Config) const override final
	{
		Config->HasKHRSurfaceExt = IsEnabled();
	}
};

class VkDebugUtilsExt : public VulkanInstanceExtension
{
public:
	VkDebugUtilsExt()
		: VulkanInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, false)
	{
	}

	bool8_t IsEnabledInConfig(const VulkanLayerExtensionConfigurations* Configs) const override final
	{
		return Configs && Configs->HasDebugUtilsExt;
	}

	void SetEnabledToConfig(VulkanLayerExtensionConfigurations* Config) const override final
	{
		Config->HasDebugUtilsExt = IsEnabled();
	}
};

class VkDebugReportExt : public VulkanInstanceExtension
{
public:
	VkDebugReportExt()
		: VulkanInstanceExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, false)
	{
	}

	bool8_t IsEnabledInConfig(const VulkanLayerExtensionConfigurations* Configs) const override final
	{
		return Configs && Configs->HasDebugReportExt;
	}

	void SetEnabledToConfig(VulkanLayerExtensionConfigurations* Config) const override final
	{
		Config->HasDebugReportExt = IsEnabled();
	}
};

class VkDebugMarkerExt : public VulkanDeviceExtension
{
public:
	VkDebugMarkerExt()
		: VulkanDeviceExtension(VK_EXT_DEBUG_MARKER_EXTENSION_NAME, false)
	{
	}

	bool8_t IsEnabledInConfig(const VulkanLayerExtensionConfigurations* Configs) const override final
	{
		return Configs && Configs->HasDebugMarkerExt;
	}

	void SetEnabledToConfig(VulkanLayerExtensionConfigurations* Config) const override final
	{
		Config->HasDebugMarkerExt = IsEnabled();
	}
};

class VkValidationFeaturesExt : public VulkanInstanceExtension
{
public:
	VkValidationFeaturesExt()
		: VulkanInstanceExtension(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME, false)
	{
	}

	bool8_t IsEnabledInConfig(const VulkanLayerExtensionConfigurations* Configs) const override final
	{
		return Configs && Configs->HasValidationFeaturesExt;
	}

	void SetEnabledToConfig(VulkanLayerExtensionConfigurations* Config) const override final
	{
		Config->HasValidationFeaturesExt = IsEnabled();
	}

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

	return WantedExts;
}

VulkanExtensionArray VulkanExtension::GetWantedDeviceExtensions()
{
	VulkanExtensionArray WantedExts;

	APPEND_EXT(VkDebugMarkerExt);

	return WantedExts;
}

#undef APPEND_EXT
#undef APPEND_LAYER

void LogEnableLayerAndExtensions(const VulkanLayerArray& Layers, const VulkanExtensionArray& Extensions, const char8_t* Category)
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
