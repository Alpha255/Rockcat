#pragma once

#include "Core/DynamicLinkLibrary.h"
#include "Engine/RHI/RHIResource.h"
#include "RHI/Vulkan/VulkanExtensionDefines.h"

#define VK_ALLOCATION_CALLBACKS nullptr

#if !defined(VULKAN_HPP_CPLUSPLUS)

#define VK_FUNC_DECLARE(Func) extern PFN_##Func Func;
#define VK_FUNC_DEFINITION(Func) PFN_##Func Func = nullptr;
#define VK_FUNC_RESET(Func) Func = nullptr;

#if USE_VK_LOADER

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

VK_EXT_FUNC_TABLE_DECLAR

#endif

class VulkanLoader : public DynamicLinkLibrary
{
public:
	~VulkanLoader();

	void LoadGlobalFuncs();
	void LoadInstanceFuncs(VkInstance Instance);
	void LoadDeviceFuncs(VkDevice Device);

	void LoadExtFuncs(VkInstance Instance, VkDevice Device, bool SupportDebugMarker, bool SupportDebugUtils);
protected:
	VulkanLoader()
		: DynamicLinkLibrary((std::string("vulkan-1") + DLL_EXTENSION).c_str())
	{
	}
private:
};

namespace VulkanResult
{
	const char* const ToString(VkResult Result);
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

template<class TInterface, class THWObject>
class VkHWObject : public std::conditional_t<std::is_void_v<TInterface>, void, TInterface>, public RHIObject<THWObject>
{
public:
	using RHIObject<THWObject>::RHIObject;
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
#else
#define VERIFY_VK(Func)                                                                                                                                \
{                                                                                                                                                      \
	vk::Result TempResult = (Func);                                                                                                                    \
	if (TempResult != vk::Result::eSuccess)                                                                                                            \
	{                                                                                                                                                  \
		LOG_CAT_CRITICAL(LogVulkanRHI, "Failed to invoke VulkanAPI: File: {}, Line: {}, vkResult: {}", __FILE__, __LINE__, vk::to_string(TempResult)); \
		assert(0);                                                                                                                                     \
	}                                                                                                                                                  \
}
#endif

class VkBaseDeviceResource
{
public:
	VkBaseDeviceResource(const class VulkanDevice& Device)
		: m_Device(Device)
	{
	}

	virtual ~VkBaseDeviceResource() {}

	FORCEINLINE const class VulkanDevice& GetDevice() const { return m_Device; }
	const vk::Device& GetNativeDevice() const;
	const vk::PhysicalDevice& GetNativePhysicalDevice() const;
	const vk::Instance& GetNativeInstance() const;
protected:
	void SetObjectName(vk::ObjectType Type, uint64_t Object, const char* Name);
private:
	const class VulkanDevice& m_Device;
};

template<class VkObject>
class VkDeviceResource : public VkBaseDeviceResource
{
public:
	using VkBaseDeviceResource::VkBaseDeviceResource;
	virtual ~VkDeviceResource() {}

	FORCEINLINE VkObject& GetNative() { return m_Native; }
	FORCEINLINE const VkObject& GetNative() const { return m_Native; }
protected:
	VkObject m_Native;
};

template<class VkObject>
class VkHwResource : public VkDeviceResource<VkObject>
{
protected:
	using BaseType = VkDeviceResource<VkObject>;
public:
	using NativeType = VkObject::NativeType;
	using BaseType::BaseType;

	void SetObjectName(const char* Name)
	{
		assert(Name);
		VkBaseDeviceResource::SetObjectName(VkObject::objectType, reinterpret_cast<uint64_t>((NativeType)BaseType::GetNative()), Name);
	}

	virtual ~VkHwResource()
	{ 
		BaseType::GetNativeDevice().destroy(BaseType::GetNative(), VK_ALLOCATION_CALLBACKS, vk::defaultDispatchLoaderDynamic);
		BaseType::GetNative() = nullptr;
	}
};
