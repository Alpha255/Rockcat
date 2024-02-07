#include "RHI/Vulkan/VulkanLoader.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanRHI.h"
#include "RHI/Vulkan/VulkanLayerExtensions.h"
#include "Engine/Services/SpdLogService.h"

#if !defined(VULKAN_HPP_CPLUSPLUS)

#define VK_LOADER_VERIFY_FUNC(Func)                                   \
	if (!Func)                                                        \
	{                                                                 \
		LOG_ERROR("Can't get entry point of function \"{}\"", #Func); \
		assert(0);                                                    \
	}

#if USE_VK_LOADER

VK_FUNC_TABLE_DEFINITION

VulkanLoader::~VulkanLoader()
{
	VK_FUNC_TABLE_RESET
}

void VulkanLoader::LoadGlobalFuncs()
{
#define VK_LOADER_LOAD_FUNCTIONS(Func)                          \
	Func = reinterpret_cast<PFN_##Func>(GetProcAddress(#Func)); \
	VK_LOADER_VERIFY_FUNC(Func)

	VK_GLOBAL_FUNC_TABLE(VK_LOADER_LOAD_FUNCTIONS)
#undef VK_LOADER_LOAD_FUNCTIONS
}

void VulkanLoader::LoadInstanceFuncs(VkInstance Instance)
{
	assert(Instance && vkGetInstanceProcAddr);

#define VK_LOADER_LOAD_FUNCTIONS(Func)                                           \
	Func = reinterpret_cast<PFN_##Func>(vkGetInstanceProcAddr(Instance, #Func)); \
	VK_LOADER_VERIFY_FUNC(Func)

	VK_INSTANCE_FUNC_TABLE(VK_LOADER_LOAD_FUNCTIONS)
#undef VK_LOADER_LOAD_FUNCTIONS
}

void VulkanLoader::LoadDeviceFuncs(VkDevice Device)
{
	assert(Device && vkGetDeviceProcAddr);

#define VK_LOADER_LOAD_FUNCTIONS(Func)                                       \
	Func = reinterpret_cast<PFN_##Func>(vkGetDeviceProcAddr(Device, #Func)); \
	VK_LOADER_VERIFY_FUNC(Func)

	VK_DEVICE_FUNC_TABLE(VK_LOADER_LOAD_FUNCTIONS)
#undef VK_LOADER_LOAD_FUNCTIONS
}

#else

VK_EXT_FUNC_TABLE_DEFINITION

VulkanLoader::~VulkanLoader()
{
	VK_EXT_FUNC_TABLE_RESET
}

void VulkanLoader::LoadExtFuncs(VkInstance Instance, VkDevice Device, bool SupportDebugMarker, bool SupportDebugUtils)
{
	assert(Instance && Device);

#define VK_LOADER_LOAD_INSTANCE_FUNCTIONS(Func)                                       \
	RHI::Func = reinterpret_cast<PFN_##Func>(vkGetInstanceProcAddr(Instance, #Func)); \
	VK_LOADER_VERIFY_FUNC(Func)

#define VK_LOADER_LOAD_DEVICE_FUNCTIONS(Func)                                     \
	RHI::Func = reinterpret_cast<PFN_##Func>(vkGetDeviceProcAddr(Device, #Func)); \
	VK_LOADER_VERIFY_FUNC(Func)

	VK_FUN_TABLE_DYNAMIC_STATE(VK_LOADER_LOAD_INSTANCE_FUNCTIONS)

	if (SupportDebugMarker)
	{
		VK_FUN_TABLE_DEBUG_MARKER(VK_LOADER_LOAD_DEVICE_FUNCTIONS)
	}
	if (SupportDebugUtils)
	{
		VK_FUN_TABLE_DEBUG_UTILS(VK_LOADER_LOAD_INSTANCE_FUNCTIONS)
	}

#undef VK_LOADER_LOAD_INSTANCE_FUNCTIONS
#undef VK_LOADER_LOAD_DEVICE_FUNCTIONS
}

#endif

namespace VulkanResult
{
	const char* const ToString(VkResult result)
	{
		switch (result)
		{
		case VK_SUCCESS:                                            return "Success";
		case VK_NOT_READY:                                          return "Not Ready";
		case VK_TIMEOUT:                                            return "Time Out";
		case VK_EVENT_SET:                                          return "Set Event";
		case VK_EVENT_RESET:                                        return "Reset Event";
		case VK_INCOMPLETE:                                         return "Incomplete";
		case VK_ERROR_OUT_OF_HOST_MEMORY:                           return "Out Of Host Memory";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:                         return "Out Of Device Memory";
		case VK_ERROR_INITIALIZATION_FAILED:                        return "Failed To Initialization";
		case VK_ERROR_DEVICE_LOST:                                  return "Lost Device";
		case VK_ERROR_MEMORY_MAP_FAILED:                            return "Failed To Map Memory";
		case VK_ERROR_LAYER_NOT_PRESENT:                            return "Layer Not Present";
		case VK_ERROR_EXTENSION_NOT_PRESENT:                        return "Extension Not Present";
		case VK_ERROR_FEATURE_NOT_PRESENT:                          return "Feature Not Present";
		case VK_ERROR_INCOMPATIBLE_DRIVER:                          return "Incompatible Driver";
		case VK_ERROR_TOO_MANY_OBJECTS:                             return "Too Many Objects";
		case VK_ERROR_FORMAT_NOT_SUPPORTED:                         return "Format Not Supported";
		case VK_ERROR_FRAGMENTED_POOL:                              return "Fragmented Pool";
		case VK_ERROR_OUT_OF_POOL_MEMORY:                           return "Out Of Pool Memory";
		case VK_ERROR_INVALID_EXTERNAL_HANDLE:                      return "Invalid External Handle";
		case VK_ERROR_SURFACE_LOST_KHR:                             return "Lost Surface";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:                     return "Window Is In Use";
		case VK_SUBOPTIMAL_KHR:                                     return "Suboptimal";
		case VK_ERROR_OUT_OF_DATE_KHR:                              return "Out Of Date";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:                     return "Incompatible Display";
		case VK_ERROR_VALIDATION_FAILED_EXT:                        return "Failed To Validation";
		case VK_ERROR_INVALID_SHADER_NV:                            return "Invalid Shader-Nvidia";
		case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "Invalid DRM Format Modifier Plane Layout";
		case VK_ERROR_FRAGMENTATION_EXT:                            return "Fragmentation";
		case VK_ERROR_NOT_PERMITTED_EXT:                            return "Not Premited";
		case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:                   return "Invalid Device Address";
		}

		return "Unknown";
	}
}

#endif

const vk::Device& VkBaseDeviceResource::GetNativeDevice() const
{
	return m_Device.GetNative();
}

const vk::PhysicalDevice& VkBaseDeviceResource::GetNativePhysicalDevice() const
{
	return m_Device.GetPhysicalDevice();
}

const vk::Instance& VkBaseDeviceResource::GetNativeInstance() const
{
	return m_Device.GetInstance();
}

void VkBaseDeviceResource::SetObjectName(vk::ObjectType Type, uint64_t Object, const char* Name)
{
	assert(Object && Name && Type != vk::ObjectType::eUnknown);

	if (VulkanRHI::GetLayerExtensionConfigs().HasDebugUtilsExt)
	{
		auto DebugUtilsObjectNameInfo = vk::DebugUtilsObjectNameInfoEXT()
			.setObjectType(Type)
			.setObjectHandle(Object)
			.setPObjectName(Name);

		VERIFY_VK(GetNativeDevice().setDebugUtilsObjectNameEXT(&DebugUtilsObjectNameInfo));
	}
	else if (VulkanRHI::GetLayerExtensionConfigs().HasDebugMarkerExt)
	{
		auto DebugMarkerObjectNameInfo = vk::DebugMarkerObjectNameInfoEXT()
			.setObjectType(GetDebugReportObjectType(Type))
			.setObject(Object)
			.setPObjectName(Name);

		VERIFY_VK(GetNativeDevice().debugMarkerSetObjectNameEXT(&DebugMarkerObjectNameInfo));
	}
}
