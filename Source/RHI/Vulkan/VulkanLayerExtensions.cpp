#include "RHI/Vulkan/VulkanLayerExtensions.h"

#define APPEND_LAYER(Type) WantedLayers.emplace_back(std::make_unique<Type>())
#define APPEND_EXT(Type) WantedExts.emplace_back(std::make_unique<Type>())

#define ADD_LAYER(Name, Supported, Needed, EnabledInConfig) \
	WantedLayers.emplace_back(std::make_unique<VulkanLayer>(Name, Supported, Needed, &Configs.EnabledInConfig))

#define ADD_EXTENSION(Name, Supported, Needed, EnabledInConfig) \
	WantedExts.emplace_back(std::make_unique<VulkanExtension>(Name, Supported, Needed, &Configs.EnabledInConfig))

VulkanLayerArray VulkanLayer::GetWantedInstanceLayers(VulkanLayerExtensionConfigurations& Configs)
{
	VulkanLayerArray WantedLayers;

	ADD_LAYER(VK_LAYER_KHRONOS_VALIDATION_NAME, true, false, HasKhronosValidationLayer);

	return WantedLayers;
}

VulkanLayerArray VulkanLayer::GetWantedDeviceLayers(VulkanLayerExtensionConfigurations&)
{
	return VulkanLayerArray();
}

VulkanExtensionArray VulkanExtension::GetWantedInstanceExtensions(VulkanLayerExtensionConfigurations& Configs)
{
	VulkanExtensionArray WantedExts;

	ADD_EXTENSION(VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_surface, true, HasKHRSurface);
#if PLATFORM_WIN32
	ADD_EXTENSION(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_win32_surface, true, HasKHRSurface);
#elif PLATFORM_LINUX
	ADD_EXTENSION(VK_KHR_XCB_SURFACE_EXTENSION_NAME, VK_KHR_surface, true, HasKHRSurface);
#else
#endif
	ADD_EXTENSION(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_EXT_debug_utils, false, HasDebugUtils);
	ADD_EXTENSION(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_EXT_debug_report, false, HasDebugReport);
	ADD_EXTENSION(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME, VK_EXT_validation_features, false, HasValidationFeatures)
		->SetOnInstanceCreation([](const VulkanLayerExtensionConfigurations& Configs, vk::InstanceCreateInfo& CreateInfo) {
				std::vector<vk::ValidationFeatureEnableEXT> EnabledFeatures;
				if (Configs.HasValidationFeatures_GPUAssisted)
				{
					EnabledFeatures.push_back(vk::ValidationFeatureEnableEXT::eGpuAssisted);
				}
				if (Configs.HasValidationFeatures_GPUAssistedReserveBindingSlot)
				{
					EnabledFeatures.push_back(vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot);
				}
				if (Configs.HasValidationFeatures_BestPractices)
				{
					EnabledFeatures.push_back(vk::ValidationFeatureEnableEXT::eBestPractices);
				}
				if (Configs.HasValidationFeatures_DebugPrintf)
				{
					EnabledFeatures.push_back(vk::ValidationFeatureEnableEXT::eDebugPrintf);
				}
				if (Configs.HasValidationFeatures_Synchronization)
				{
					EnabledFeatures.push_back(vk::ValidationFeatureEnableEXT::eSynchronizationValidation);
				}

				auto ValidationFeatures = vk::ValidationFeaturesEXT()
					.setEnabledValidationFeatures(EnabledFeatures);
				SetPNext(CreateInfo, ValidationFeatures);
			});

	ADD_EXTENSION(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_KHR_get_physical_device_properties2, false, HasGetPhysicalDeviceProperties2);

	return WantedExts;
}

VulkanExtensionArray VulkanExtension::GetWantedDeviceExtensions(VulkanLayerExtensionConfigurations& Configs)
{
	VulkanExtensionArray WantedExts;

	ADD_EXTENSION(VK_EXT_DEBUG_MARKER_EXTENSION_NAME, VK_EXT_debug_marker, false, HasDebugMarker);
	ADD_EXTENSION(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME, VK_KHR_timeline_semaphore, false, HasTimelineSemaphore);
	ADD_EXTENSION(VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME, VK_EXT_full_screen_exclusive, false, HasFullscreenExclusive);
	ADD_EXTENSION(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME, VK_KHR_depth_stencil_resolve, false, HasDepthStencilResolve);
	ADD_EXTENSION(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME, VK_EXT_extended_dynamic_state, false, HasDynamicState);
	
	ADD_EXTENSION(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME, VK_KHR_create_renderpass2, false, HasRenderPass2);
	ADD_EXTENSION(VK_EXT_HOST_IMAGE_COPY_EXTENSION_NAME, VK_EXT_host_image_copy, false, HasHostImageCopy);
	ADD_EXTENSION(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, VK_KHR_dynamic_rendering, false, HasDynamicRendering);
	ADD_EXTENSION(VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME, VK_EXT_graphics_pipeline_library, false, HasGraphicsPipelineLibrary);
	ADD_EXTENSION(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME, VK_EXT_inline_uniform_block, false, HasInlineUniformBlock);
	ADD_EXTENSION(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME, VK_EXT_conservative_rasterization, false, HasConservativeRasterization);
	ADD_EXTENSION(VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME, VK_EXT_conditional_rendering, false, HasConditionalRendering);
	ADD_EXTENSION(VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME, VK_KHR_shader_non_semantic_info, false, HasDebugPrintf);

	ADD_EXTENSION(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME, VK_KHR_push_descriptor, false, HasPushDescriptor);

	ADD_EXTENSION(VK_KHR_MAINTENANCE_1_EXTENSION_NAME, VK_KHR_maintenance1, false, HasMaintenance1);
	ADD_EXTENSION(VK_KHR_MAINTENANCE_2_EXTENSION_NAME, VK_KHR_maintenance2, false, HasMaintenance2);
	ADD_EXTENSION(VK_KHR_MAINTENANCE_3_EXTENSION_NAME, VK_KHR_maintenance3, false, HasMaintenance3);
	ADD_EXTENSION(VK_KHR_MAINTENANCE_4_EXTENSION_NAME, VK_KHR_maintenance4, false, HasMaintenance4);
	ADD_EXTENSION(VK_KHR_MAINTENANCE_5_EXTENSION_NAME, VK_KHR_maintenance5, false, HasMaintenance5);
	ADD_EXTENSION(VK_KHR_MAINTENANCE_6_EXTENSION_NAME, VK_KHR_maintenance6, false, HasMaintenance6);

	return WantedExts;
}

#undef APPEND_EXT
#undef APPEND_LAYER

void LogEnabledLayerAndExtensions(const VulkanLayerArray& Layers, const VulkanExtensionArray& Extensions, const char* Category)
{
	for (const auto &Layer : Layers)
	{
		if (Layer->IsNeeded())
		{
			if (Layer->IsEnabled())
			{
				LOG_CAT_INFO(LogVulkanRHI, "Enable {} layer: \"{}\"", Category, Layer->GetName());
			}
			else
			{
				LOG_CAT_ERROR(LogVulkanRHI, "Requested {} layer \"{}\" is not supported.", Category, Layer->GetName());
			}
		}
	}

	for (const auto& Extension : Extensions)
	{
		if (Extension->IsNeeded())
		{
			if (Extension->IsEnabled())
			{
				LOG_CAT_INFO(LogVulkanRHI, "Enable {} extension: \"{}\"", Category, Extension->GetName());
			}
			else
			{
				LOG_CAT_ERROR(LogVulkanRHI, "Requested {} layer \"{}\" is not supported.", Category, Extension->GetName());
			}
		}
	}
}
