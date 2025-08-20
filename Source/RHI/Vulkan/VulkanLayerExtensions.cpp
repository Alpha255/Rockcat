#include "RHI/Vulkan/VulkanLayerExtensions.h"

#define APPEND_LAYER(Type) WantedLayers.emplace_back(std::make_unique<Type>())
#define APPEND_EXT(Type) WantedExts.emplace_back(std::make_unique<Type>())

#define ADD_LAYER(Name, Supported, Needed, EnabledInConfig) \
	WantedLayers.emplace_back(std::make_unique<VulkanLayer>(Name, Supported, Needed, &Settings.EnabledInConfig))

#define ADD_EXTENSION(Name, Supported, Needed, EnabledInConfig) \
	WantedExts.emplace_back(std::make_unique<VulkanExtension>(Name, Supported, Needed, &Settings.EnabledInConfig))

VulkanLayerArray VulkanLayer::GetWantedInstanceLayers(VulkanExtensionSettings& Settings)
{
	VulkanLayerArray WantedLayers;

	ADD_LAYER(VK_LAYER_KHRONOS_VALIDATION_NAME, true, false, KhronosValidationLayer);

	return WantedLayers;
}

VulkanLayerArray VulkanLayer::GetWantedDeviceLayers(VulkanExtensionSettings&)
{
	return VulkanLayerArray();
}

VulkanExtensionArray VulkanExtension::GetWantedInstanceExtensions(VulkanExtensionSettings& Settings)
{
	VulkanExtensionArray WantedExts;

	ADD_EXTENSION(VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_surface, true, KHRSurface);
#if PLATFORM_WIN32
	ADD_EXTENSION(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_win32_surface, true, KHRSurface);
#elif PLATFORM_LINUX
	ADD_EXTENSION(VK_KHR_XCB_SURFACE_EXTENSION_NAME, VK_KHR_surface, true, KHRSurface);
#else
#endif
	ADD_EXTENSION(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_EXT_debug_utils, false, DebugUtils);
	ADD_EXTENSION(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_EXT_debug_report, false, DebugReport);
	ADD_EXTENSION(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME, VK_EXT_validation_features, false, ValidationFeatures)
		->SetOnInstanceCreation([](const VulkanExtensionSettings& Settings, vk::InstanceCreateInfo& CreateInfo) {
				std::vector<vk::ValidationFeatureEnableEXT> EnabledFeatures;
				if (Settings.ValidationFeatures_GPUAssisted)
				{
					EnabledFeatures.push_back(vk::ValidationFeatureEnableEXT::eGpuAssisted);
				}
				if (Settings.ValidationFeatures_GPUAssistedReserveBindingSlot)
				{
					EnabledFeatures.push_back(vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot);
				}
				if (Settings.ValidationFeatures_BestPractices)
				{
					EnabledFeatures.push_back(vk::ValidationFeatureEnableEXT::eBestPractices);
				}
				if (Settings.ValidationFeatures_DebugPrintf)
				{
					EnabledFeatures.push_back(vk::ValidationFeatureEnableEXT::eDebugPrintf);
				}
				if (Settings.ValidationFeatures_Synchronization)
				{
					EnabledFeatures.push_back(vk::ValidationFeatureEnableEXT::eSynchronizationValidation);
				}

				auto ValidationFeatures = vk::ValidationFeaturesEXT()
					.setEnabledValidationFeatures(EnabledFeatures);
				SetPNext(CreateInfo, ValidationFeatures);
			});

	ADD_EXTENSION(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_KHR_get_physical_device_properties2, false, GetPhysicalDeviceProperties2);

	return WantedExts;
}

VulkanExtensionArray VulkanExtension::GetWantedDeviceExtensions(VulkanExtensionSettings& Settings)
{
	VulkanExtensionArray WantedExts;

	ADD_EXTENSION(VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_swapchain, true, KHRSurface);
	ADD_EXTENSION(VK_EXT_DEBUG_MARKER_EXTENSION_NAME, VK_EXT_debug_marker, false, DebugMarker);
	ADD_EXTENSION(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME, VK_KHR_timeline_semaphore, false, TimelineSemaphore);
	ADD_EXTENSION(VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME, VK_EXT_full_screen_exclusive, false, FullscreenExclusive);
	ADD_EXTENSION(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME, VK_KHR_depth_stencil_resolve, false, DepthStencilResolve);
	ADD_EXTENSION(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME, VK_EXT_extended_dynamic_state, false, DynamicState);
	
	ADD_EXTENSION(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME, VK_KHR_create_renderpass2, false, RenderPass2);
	ADD_EXTENSION(VK_EXT_HOST_IMAGE_COPY_EXTENSION_NAME, VK_EXT_host_image_copy, false, HostImageCopy);
	ADD_EXTENSION(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, VK_KHR_dynamic_rendering, false, DynamicRendering);
	ADD_EXTENSION(VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME, VK_EXT_graphics_pipeline_library, false, GraphicsPipelineLibrary);
	ADD_EXTENSION(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME, VK_EXT_inline_uniform_block, false, InlineUniformBlock);
	ADD_EXTENSION(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME, VK_EXT_conservative_rasterization, false, ConservativeRasterization);
	ADD_EXTENSION(VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME, VK_EXT_conditional_rendering, false, ConditionalRendering);
	ADD_EXTENSION(VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME, VK_KHR_shader_non_semantic_info, false, DebugPrintf);

	ADD_EXTENSION(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME, VK_KHR_push_descriptor, false, PushDescriptor);

	ADD_EXTENSION(VK_KHR_MAINTENANCE_1_EXTENSION_NAME, VK_KHR_maintenance1, false, Maintenance1);
	ADD_EXTENSION(VK_KHR_MAINTENANCE_2_EXTENSION_NAME, VK_KHR_maintenance2, false, Maintenance2);
	ADD_EXTENSION(VK_KHR_MAINTENANCE_3_EXTENSION_NAME, VK_KHR_maintenance3, false, Maintenance3);
	ADD_EXTENSION(VK_KHR_MAINTENANCE_4_EXTENSION_NAME, VK_KHR_maintenance4, false, Maintenance4);
	ADD_EXTENSION(VK_KHR_MAINTENANCE_5_EXTENSION_NAME, VK_KHR_maintenance5, false, Maintenance5);
	ADD_EXTENSION(VK_KHR_MAINTENANCE_6_EXTENSION_NAME, VK_KHR_maintenance6, false, Maintenance6);

	return WantedExts;
}

#undef APPEND_EXT
#undef APPEND_LAYER

void LogEnabledLayerAndExtensions(const VulkanLayerArray& Layers, const VulkanExtensionArray& Extensions, const char* Category)
{
	for (const auto &Layer : Layers)
	{
		if (Layer->IsEnabled())
		{
			LOG_CAT_INFO(LogVulkanRHI, "Enable {} layer: \"{}\", spec version: {}", Category, Layer->GetName(), Layer->GetSpecVersion());
		}
		else
		{
			if (Layer->IsNeeded())
			{
				LOG_CAT_ERROR(LogVulkanRHI, "Requested {} layer \"{}\" is not supported.", Category, Layer->GetName());
			}
		}
	}

	for (const auto& Extension : Extensions)
	{
		if (Extension->IsEnabled())
		{
			LOG_CAT_INFO(LogVulkanRHI, "Enable {} extension: \"{}\", spec version: {}", Category, Extension->GetName(), Extension->GetSpecVersion());
		}
		else
		{
			if (Extension->IsNeeded())
			{
				LOG_CAT_ERROR(LogVulkanRHI, "Requested {} layer \"{}\" is not supported.", Category, Extension->GetName());
			}
		}
	}
}
