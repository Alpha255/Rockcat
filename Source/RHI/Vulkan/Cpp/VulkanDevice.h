#pragma once

#include "VulkanInstance.h"
#include "Runtime/Engine/RHI/RHIDevice.h"

NAMESPACE_START(RHI)

class VulkanDevice final
{
public:
	VulkanDevice(vk::Instance* Instance, bool8_t EnableComputeQueue, bool8_t EnableTransferQueue)
	{
		assert(Instance);

		uint32_t GraphicsQueueIndex = ~0u, ComputeQueueIndex = ~0u, TransferQueueIndex = ~0u, PresentQueueIndex = ~0u;

		std::vector<vk::PhysicalDevice> DiscreteGpus;
		std::vector<vk::PhysicalDevice> OtherGpus;
		for (auto PhysicalDevice : Instance->enumeratePhysicalDevices())
		{
			if (GetQueueFamilyIndex(&PhysicalDevice, GraphicsQueueIndex, ComputeQueueIndex, TransferQueueIndex, PresentQueueIndex))
			{
				if (PhysicalDevice.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
				{
					DiscreteGpus.push_back(PhysicalDevice);
				}
				else
				{
					OtherGpus.push_back(PhysicalDevice);
				}
			}
		}

		if (DiscreteGpus.size() == 0u && OtherGpus.size() > 0u)
		{
			LOG_WARNING("Can't find discrete GPU");
		}

		m_PhysicalDevice = DiscreteGpus.size() > 0u ? DiscreteGpus[0] : (OtherGpus.size() > 0u ? OtherGpus[0] : vk::PhysicalDevice{});
		assert(m_PhysicalDevice);

		GetQueueFamilyIndex(&m_PhysicalDevice, GraphicsQueueIndex, ComputeQueueIndex, TransferQueueIndex, PresentQueueIndex);

		std::vector<std::string> TryToEnableLayers
		{
	#if defined(_DEBUG)
	#endif
		};
		std::vector<const char8_t*> EnabledLayers;

		std::vector<std::string> TryToEnableExtensions
		{
	#if VK_KHR_swapchain
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	#endif
	#if VK_EXT_debug_marker
			VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
	#endif
	#if VK_EXT_descriptor_indexing  /// Bindless???
			/// VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
	#endif
	#if VK_KHR_buffer_device_address
			/// VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
	#endif
	#if VK_KHR_timeline_semaphore
			/// VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
	#endif
	#if VK_KHR_synchronization2
			/// VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
	#endif
	#if VK_KHR_create_renderpass2
			/// VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
	#endif
	
	#if VK_EXT_extended_dynamic_state
			/// VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME
	#endif
	
	#if VK_EXT_extended_dynamic_state2
			/// VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME
	#endif
	#if VK_KHR_dynamic_rendering
	#endif
	#if VK_KHR_maintenance1
			VK_KHR_MAINTENANCE1_EXTENSION_NAME,
	#endif
		};
		std::vector<const char8_t*> EnabledExtensions;

		for (const auto& LayerProperty : m_PhysicalDevice.enumerateDeviceLayerProperties())
		{
			if (std::find(TryToEnableLayers.begin(), TryToEnableLayers.end(), std::string(LayerProperty.layerName)) != TryToEnableLayers.end())
			{
				EnabledLayers.push_back(LayerProperty.layerName.data());
			}
		}

		for (const auto& ExtensionProperty : m_PhysicalDevice.enumerateDeviceExtensionProperties())
		{
			if (std::find(TryToEnableExtensions.begin(), TryToEnableExtensions.end(), std::string(ExtensionProperty.extensionName)) != TryToEnableExtensions.end())
			{
				EnabledExtensions.push_back(ExtensionProperty.extensionName.data());
			}
		}

		std::unordered_set<uint32_t> QueueFamilyIndices{ GraphicsQueueIndex, PresentQueueIndex };
		if (EnableComputeQueue)
		{
			QueueFamilyIndices.insert(ComputeQueueIndex);
		}
		if (EnableTransferQueue)
		{
			QueueFamilyIndices.insert(TransferQueueIndex);
		}

		const float32_t Priority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> QueueCreateInfos;
		for (auto QueueFamilyIndex : QueueFamilyIndices)
		{
			QueueCreateInfos.emplace_back(std::move(
				vk::DeviceQueueCreateInfo()
					.setQueueFamilyIndex(QueueFamilyIndex)
					.setQueueCount(1u)
					.setPQueuePriorities(&Priority)));
		}

		auto DeviceFeatures = m_PhysicalDevice.getFeatures();

		auto CreateInfo = vk::DeviceCreateInfo()
			.setQueueCreateInfoCount(static_cast<uint32_t>(QueueCreateInfos.size()))
			.setPQueueCreateInfos(QueueCreateInfos.data())
			.setEnabledLayerCount(static_cast<uint32_t>(EnabledLayers.size()))
			.setPpEnabledLayerNames(EnabledLayers.data())
			.setEnabledExtensionCount(static_cast<uint32_t>(EnabledExtensions.size()))
			.setPpEnabledExtensionNames(EnabledExtensions.data())
			.setPEnabledFeatures(&DeviceFeatures);

		VERIFY_VK(m_PhysicalDevice.createDevice(&CreateInfo, nullptr, &m_Device));

		auto DeviceProperties = m_PhysicalDevice.getProperties();
		LOG_INFO("Create {} Vulkan device on adapter: \"{} {}\", DeviceID = {}. VulkanAPI Version: {}.{}.{}",
			vk::to_string(DeviceProperties.deviceType).data(),
			RHIDevice::GetVendorName(DeviceProperties.vendorID),
			DeviceProperties.deviceName.data(),
			DeviceProperties.deviceID,
			VK_VERSION_MAJOR(DeviceProperties.apiVersion),
			VK_VERSION_MINOR(DeviceProperties.apiVersion),
			VK_VERSION_PATCH(DeviceProperties.apiVersion));

		VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Device);
	}

	~VulkanDevice()
	{
		m_Device.destroy();
		m_Device = nullptr;
	}
protected:
	bool8_t GetQueueFamilyIndex(
		const vk::PhysicalDevice* PhysicalDevice, 
		uint32_t& GraphicsQueueIndex, 
		uint32_t& ComputeQueueIndex, 
		uint32_t& TransferQueueIndex, 
		uint32_t& PresentQueueIndex) const
	{
		assert(PhysicalDevice);

#define IS_INVALID(Index) Property.queueCount > 0u && Index == std::numeric_limits<uint32_t>().max()
#define IS_VALID(Index) Index == std::numeric_limits<uint32_t>().max()
#define IS_FLAGS_MATCH(Flag) (Property.queueFlags & vk::QueueFlagBits::Flag) == vk::QueueFlagBits::Flag
		GraphicsQueueIndex = ComputeQueueIndex = TransferQueueIndex = std::numeric_limits<uint32_t>().max();

		auto Properties = PhysicalDevice->getQueueFamilyProperties();
		for (uint32_t Index = 0u; Index < Properties.size(); ++Index)
		{
			const auto& Property = Properties[Index];
			if (IS_INVALID(GraphicsQueueIndex) && IS_FLAGS_MATCH(eGraphics))
			{
				GraphicsQueueIndex = Index;
			}
			if (IS_INVALID(ComputeQueueIndex) && IS_FLAGS_MATCH(eCompute) && !(IS_FLAGS_MATCH(eGraphics)))
			{
				ComputeQueueIndex = Index;
			}
			if (IS_INVALID(TransferQueueIndex) && IS_FLAGS_MATCH(eTransfer) && !(IS_FLAGS_MATCH(eGraphics)) && !(IS_FLAGS_MATCH(eCompute)))
			{
				TransferQueueIndex = Index;
			}
			if (IS_INVALID(PresentQueueIndex))
			{
#if PLATFORM_WIN32
				if (PhysicalDevice->getWin32PresentationSupportKHR(Index))
				{
					PresentQueueIndex = Index;
				}
#endif
			}
		}
#undef IS_VALID
#undef IS_INVALID
#undef IS_FLAGS_MATCH

		return IS_VALID(GraphicsQueueIndex) && IS_VALID(ComputeQueueIndex) && IS_VALID(TransferQueueIndex) && IS_VALID(PresentQueueIndex);
	}
private:
	vk::PhysicalDevice m_PhysicalDevice;
	vk::Device m_Device;
};

NAMESPACE_END(RHI)
