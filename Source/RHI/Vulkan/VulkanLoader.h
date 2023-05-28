#pragma once

#include "Runtime/Core/DynamicLinkLibrary.h"
#include "Runtime/Engine/RHI/RHIInterface.h"

#define USE_VK_LOADER true

#define VK_FUNC_DECLARE(Func) extern PFN_##Func Func;
#define VK_FUNC_DEFINITION(Func) PFN_##Func Func = nullptr;
#define VK_FUNC_RESET(Func) Func = nullptr;

#if USE_VK_LOADER

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#define VK_GLOBAL_FUNC_TABLE(Action)               \
	Action(vkCreateInstance)                       \
	Action(vkGetDeviceQueue)                       \
	Action(vkCreateCommandPool)                    \
	Action(vkGetDeviceProcAddr)                    \
	Action(vkGetInstanceProcAddr)                  \
	Action(vkEnumerateDeviceLayerProperties)       \
	Action(vkEnumerateInstanceLayerProperties)     \
	Action(vkEnumerateInstanceExtensionProperties) 

#define VK_INSTANCE_FUNC_TABLE_GENERAL(Action)        \
	Action(vkCreateDevice)                            \
	Action(vkDestroyInstance)                         \
	Action(vkCmdSetCullModeEXT)                       \
	Action(vkDestroySurfaceKHR)                       \
	Action(vkSetHdrMetadataEXT)                       \
	Action(vkCmdDebugMarkerEndEXT)                    \
	Action(vkCmdDebugMarkerBeginEXT)                  \
	Action(vkCmdDebugMarkerInsertEXT)                 \
	Action(vkEnumeratePhysicalDevices)                \
	Action(vkGetPhysicalDeviceFeatures)               \
	Action(vkSetDebugUtilsObjectNameEXT)              \
	Action(vkDebugMarkerSetObjectNameEXT)             \
	Action(vkGetPhysicalDeviceProperties)             \
	Action(vkGetPhysicalDeviceFormatProperties)       \
	Action(vkGetPhysicalDeviceMemoryProperties)       \
	Action(vkEnumerateDeviceExtensionProperties)      \
	Action(vkGetPhysicalDeviceSurfaceSupportKHR)      \
	Action(vkGetPhysicalDeviceSurfaceFormatsKHR)      \
	Action(vkGetPhysicalDeviceQueueFamilyProperties)  \
	Action(vkGetPhysicalDeviceSurfaceCapabilitiesKHR) \
	Action(vkGetPhysicalDeviceSurfacePresentModesKHR) 

#define VK_DEVICE_FUNC_TABLE(Action)       \
	Action(vkCmdDraw)                      \
	Action(vkSetEvent)                     \
	Action(vkMapMemory)                    \
	Action(vkFreeMemory)                   \
	Action(vkResetEvent)                   \
	Action(vkCmdDispatch)                  \
	Action(vkResetFences)                  \
	Action(vkUnmapMemory)                  \
	Action(vkCreateImage)                  \
	Action(vkCreateEvent)                  \
	Action(vkCreateFence)                  \
	Action(vkQueueSubmit)                  \
	Action(vkCmdCopyImage)                 \
	Action(vkDestroyEvent)                 \
	Action(vkCreateBuffer)                 \
	Action(vkDestroyImage)                 \
	Action(vkDestroyFence)                 \
	Action(vkQueueWaitIdle)                \
	Action(vkDestroyDevice)                \
	Action(vkCmdSetScissor)                \
	Action(vkDestroyBuffer)                \
	Action(vkWaitForFences)                \
	Action(vkCmdCopyBuffer)                \
	Action(vkCreateSampler)                \
	Action(vkWaitSemaphores)               \
	Action(vkCmdDrawIndexed)               \
	Action(vkAllocateMemory)               \
	Action(vkCmdSetViewport)               \
	Action(vkDestroySampler)               \
	Action(vkGetFenceStatus)               \
	Action(vkDeviceWaitIdle)               \
	Action(vkGetEventStatus)               \
	Action(vkSignalSemaphore)              \
	Action(vkBindImageMemory)              \
	Action(vkDestroyPipeline)              \
	Action(vkQueuePresentKHR)              \
	Action(vkCmdDrawIndirect)              \
	Action(vkCreateSemaphore)              \
	Action(vkCreateImageView)              \
	Action(vkCmdBindPipeline)              \
	Action(vkCmdPushConstants)             \
	Action(vkResetCommandPool)             \
	Action(vkBindBufferMemory)             \
	Action(vkDestroyImageView)             \
	Action(vkCmdEndRenderPass)             \
	Action(vkCreateRenderPass)             \
	Action(vkEndCommandBuffer)             \
	Action(vkDestroySemaphore)             \
	Action(vkDestroyRenderPass)            \
	Action(vkCreateFramebuffer)            \
	Action(vkCmdExecuteCommands)           \
	Action(vkDestroyFramebuffer)           \
	Action(vkCmdBeginRenderPass)           \
	Action(vkResetCommandBuffer)           \
	Action(vkBeginCommandBuffer)           \
	Action(vkCmdPipelineBarrier)           \
	Action(vkCmdClearColorImage)           \
	Action(vkFreeCommandBuffers)           \
	Action(vkDestroyCommandPool)           \
	Action(vkCreateSwapchainKHR)           \
	Action(vkCreateShaderModule)           \
	Action(vkCmdBindIndexBuffer)           \
	Action(vkResetDescriptorPool)          \
	Action(vkCmdBeginRenderPass2)          \
	Action(vkAcquireNextImageKHR)          \
	Action(vkDestroySwapchainKHR)          \
	Action(vkDestroyShaderModule)          \
	Action(vkCreatePipelineCache)          \
	Action(vkCmdDispatchIndirect)          \
	Action(vkCmdBindVertexBuffers)         \
	Action(vkCmdCopyBufferToImage)         \
	Action(vkCreateDescriptorPool)         \
	Action(vkUpdateDescriptorSets)         \
	Action(vkCreatePipelineLayout)         \
	Action(vkDestroyPipelineCache)         \
	Action(vkCmdBindDescriptorSets)        \
	Action(vkDestroyDescriptorPool)        \
	Action(vkDestroyPipelineLayout)        \
	Action(vkGetSwapchainImagesKHR)        \
	Action(vkCmdDrawIndexedIndirect)       \
	Action(vkAllocateCommandBuffers)       \
	Action(vkAllocateDescriptorSets)       \
	Action(vkCreateGraphicsPipelines)      \
	Action(vkFlushMappedMemoryRanges)      \
	Action(vkGetSemaphoreCounterValue)     \
	Action(vkCmdClearDepthStencilImage)    \
	Action(vkCreateDescriptorSetLayout)    \
	Action(vkGetImageMemoryRequirements)   \
	Action(vkDestroyDescriptorSetLayout)   \
	Action(vkGetBufferMemoryRequirements)  \
	Action(vkInvalidateMappedMemoryRanges) \

#if defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_INSTANCE_FUNC_TABLE_PLATFORM(Action) \
	Action(vkCreateWin32SurfaceKHR)
#else
	#error Unknown platform!
#endif
	
#define VK_INSTANCE_FUNC_TABLE(Action)     \
	VK_INSTANCE_FUNC_TABLE_GENERAL(Action) \
	VK_INSTANCE_FUNC_TABLE_PLATFORM(Action) 

#define VK_FUNC_TABLE(Action)      \
	VK_GLOBAL_FUNC_TABLE(Action)   \
	VK_INSTANCE_FUNC_TABLE(Action) \
	VK_DEVICE_FUNC_TABLE(Action)

#define VK_FUNC_TABLE_DECLARE    VK_FUNC_TABLE(VK_FUNC_DECLARE)
#define VK_FUNC_TABLE_DEFINITION VK_FUNC_TABLE(VK_FUNC_DEFINITION)
#define VK_FUNC_TABLE_RESET      VK_FUNC_TABLE(VK_FUNC_RESET)

VK_FUNC_TABLE_DECLARE

#else

#include <vulkan/vulkan.h>

#define VK_FUN_TABLE_DEBUG_MARKER(Action) \
	Action(vkDebugMarkerSetObjectTagEXT)  \
	Action(vkDebugMarkerSetObjectNameEXT) \
	Action(vkCmdDebugMarkerBeginEXT)      \
	Action(vkCmdDebugMarkerEndEXT)        \
	Action(vkCmdDebugMarkerInsertEXT)

#define VK_FUN_TABLE_DEBUG_UTILS(Action)    \
	Action(vkSetDebugUtilsObjectNameEXT)    \
	Action(vkSetDebugUtilsObjectTagEXT)     \
	Action(vkQueueBeginDebugUtilsLabelEXT)  \
	Action(vkQueueEndDebugUtilsLabelEXT)    \
	Action(vkQueueInsertDebugUtilsLabelEXT) \
	Action(vkCmdBeginDebugUtilsLabelEXT)    \
	Action(vkCmdEndDebugUtilsLabelEXT)      \
	Action(vkCmdInsertDebugUtilsLabelEXT)   \
	Action(vkCreateDebugUtilsMessengerEXT)  \
	Action(vkDestroyDebugUtilsMessengerEXT) \
	Action(vkSubmitDebugUtilsMessageEXT)

#define VK_FUN_TABLE_DYNAMIC_STATE(Action)   \
	Action(vkCmdSetCullModeEXT)              \
	Action(vkCmdSetFrontFaceEXT)             \
	Action(vkCmdSetPrimitiveTopologyEXT)     \
	Action(vkCmdSetViewportWithCountEXT)     \
	Action(vkCmdSetScissorWithCountEXT)      \
	Action(vkCmdBindVertexBuffers2EXT)       \
	Action(vkCmdSetDepthTestEnableEXT)       \
	Action(vkCmdSetDepthWriteEnableEXT)      \
	Action(vkCmdSetDepthCompareOpEXT)        \
	Action(vkCmdSetDepthBoundsTestEnableEXT) \
	Action(vkCmdSetStencilTestEnableEXT)     \
	Action(vkCmdSetStencilOpEXT)

#define VK_EXT_FUNC_TABLE(Action)      \
	VK_FUN_TABLE_DEBUG_MARKER(Action)  \
	VK_FUN_TABLE_DEBUG_UTILS(Action)   \
	VK_FUN_TABLE_DYNAMIC_STATE(Action)

#define VK_EXT_FUNC_TABLE_DECLAR      VK_EXT_FUNC_TABLE(VK_FUNC_DECLARE)
#define VK_EXT_FUNC_TABLE_DEFINITION  VK_EXT_FUNC_TABLE(VK_FUNC_DEFINITION)
#define VK_EXT_FUNC_TABLE_RESET       VK_EXT_FUNC_TABLE(VK_FUNC_RESET)

NAMESPACE_START(RHI)
	VK_EXT_FUNC_TABLE_DECLAR
NAMESPACE_END(RHI)

#endif

NAMESPACE_START(RHI)

class VulkanLoader : public DynamicLinkLibrary
{
public:
	~VulkanLoader();

	void LoadGlobalFuncs();
	void LoadInstanceFuncs(VkInstance Instance);
	void LoadDeviceFuncs(VkDevice Device);

	void LoadExtFuncs(VkInstance Instance, VkDevice Device, bool8_t SupportDebugMarker, bool8_t SupportDebugUtils);
protected:
	VulkanLoader()
		: DynamicLinkLibrary((std::string("vulkan-1") + DLL_EXTENSION).c_str())
	{
	}
private:
};

namespace VulkanResult
{
	const char8_t* const ToString(VkResult Result);
};

#define VERIFY_VK(Func)                                                                      \
{                                                                                            \
	VkResult Result = (Func);                                                                \
	if (Result != VK_SUCCESS)                                                                \
	{                                                                                        \
		LOG_ERROR("Vulkan: Failed to invoke VulkanAPI: {}", VulkanResult::ToString(Result)); \
		assert(0);                                                                           \
	}                                                                                        \
}

#define VK_ALLOCATION_CALLBACKS nullptr

template<class TInterface, class THWObject>
class VkHWObject : public std::conditional_t<std::is_void_v<TInterface>, void, TInterface>, public IHWObject<THWObject>
{
public:
	using IHWObject<THWObject>::IHWObject;
	using InterfaceType = std::conditional_t<std::is_void_v<TInterface>, void, TInterface>;

	VkHWObject() = default;

	VkHWObject(class VulkanDevice* Device)
		: m_Device(Device)
	{
	}

	template<class... TArgs>
	VkHWObject(class VulkanDevice* Device, TArgs&&... Args)
		: InterfaceType(std::forward<TArgs>(Args)...)
		, m_Device(Device)
	{
	}

	class VulkanDevice* Device()
	{
		return m_Device;
	}
protected:
	class VulkanDevice* m_Device = nullptr;
};

NAMESPACE_END(RHI)