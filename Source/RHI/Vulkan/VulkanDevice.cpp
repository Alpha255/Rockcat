#if 0
#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanInstance.h"

NAMESPACE_START(RHI)

#if VK_EXT_debug_report
	static_assert(VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT == VK_OBJECT_TYPE_SEMAPHORE, "Miss match");
	static_assert(VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT == VK_OBJECT_TYPE_FENCE, "Miss match");
	static_assert(VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT == VK_OBJECT_TYPE_BUFFER, "Miss match");
	static_assert(VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT == VK_OBJECT_TYPE_IMAGE, "Miss match");
	static_assert(VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT == VK_OBJECT_TYPE_EVENT, "Miss match");
	static_assert(VK_OBJECT_TYPE_QUERY_POOL == VK_OBJECT_TYPE_QUERY_POOL, "Miss match");
	static_assert(VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT == VK_OBJECT_TYPE_BUFFER_VIEW, "Miss match");
	static_assert(VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT == VK_OBJECT_TYPE_IMAGE_VIEW, "Miss match");
	static_assert(VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT == VK_OBJECT_TYPE_SHADER_MODULE, "Miss match");
	static_assert(VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT == VK_OBJECT_TYPE_SAMPLER, "Miss match");
#endif

	VulkanDevice::VulkanDevice(VulkanInstance* Instance)
	: m_Instance(Instance->Get())
{
	assert(m_Instance);

	uint32_t Count = 0u;
	VERIFY_VK(vkEnumeratePhysicalDevices(m_Instance, &Count, nullptr));
	assert(Count > 0u);

	std::vector<VkPhysicalDevice> PhysicalDevices(Count);
	VERIFY_VK(vkEnumeratePhysicalDevices(m_Instance, &Count, PhysicalDevices.data()));

	uint32_t GpuIndex = std::numeric_limits<uint32_t>().max();
	uint32_t DiscreteGpuIndex = std::numeric_limits<uint32_t>().max();
	VkPhysicalDeviceType GpuType = VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM;

	uint32_t GraphicsQueueFamilyIndex = ~0u;
	uint32_t ComputeQueueFamilyIndex = ~0u;
	uint32_t TransferQueueFamilyIndex = ~0u;
	uint32_t NumQueuePriority = 0u;

	auto GetGpuTypeName = [](VkPhysicalDeviceType Type)-> const char8_t* const
	{
		switch (Type)
		{
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:          return "Other";
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "Integrated Gpu";
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return "Discrete Gpu";
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return "Virtual Gpu";
		case VK_PHYSICAL_DEVICE_TYPE_CPU:            return "Cpu";
		}

		return "Unknown";
	};

	std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;
	for (uint32_t DeviceIndex = 0u; DeviceIndex < PhysicalDevices.size(); ++DeviceIndex)
	{
		VkPhysicalDeviceProperties Properties;
		vkGetPhysicalDeviceProperties(PhysicalDevices[DeviceIndex], &Properties);
		GpuType = Properties.deviceType;

		GpuIndex = DeviceIndex;
		DiscreteGpuIndex = GpuType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? DeviceIndex : DiscreteGpuIndex;

		vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevices[DeviceIndex], &Count, nullptr);
		assert(Count > 0u);

		std::vector<VkQueueFamilyProperties> QueueFamilyProperties(Count);
		vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevices[DeviceIndex], &Count, QueueFamilyProperties.data());

		for (uint32_t FamilyPropertyIndex = 0u; FamilyPropertyIndex < QueueFamilyProperties.size(); ++FamilyPropertyIndex)
		{
			if ((static_cast<VkQueueFlagBits>(QueueFamilyProperties[FamilyPropertyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) &&
				GraphicsQueueFamilyIndex == ~0u)
			{
				GraphicsQueueFamilyIndex = FamilyPropertyIndex;
			}

			if ((static_cast<VkQueueFlagBits>(QueueFamilyProperties[FamilyPropertyIndex].queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT) &&
				ComputeQueueFamilyIndex == ~0u &&
				ComputeQueueFamilyIndex != FamilyPropertyIndex &&
				GraphicsQueueFamilyIndex != FamilyPropertyIndex)
			{
				ComputeQueueFamilyIndex = FamilyPropertyIndex;
			}

			if ((static_cast<VkQueueFlagBits>(QueueFamilyProperties[FamilyPropertyIndex].queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) &&
				TransferQueueFamilyIndex == ~0u &&
				TransferQueueFamilyIndex != FamilyPropertyIndex &&
				((QueueFamilyProperties[FamilyPropertyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
				((QueueFamilyProperties[FamilyPropertyIndex].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
			{
				continue;
				/// TransferQueueFamilyIndex = FamilyPropertyIndex;
			}

			if (GraphicsQueueFamilyIndex == ~0u &&
				ComputeQueueFamilyIndex == ~0u &&
				TransferQueueFamilyIndex == ~0u)
			{
				continue;
			}

			NumQueuePriority += QueueFamilyProperties[FamilyPropertyIndex].queueCount;
			VkDeviceQueueCreateInfo QueueCreateInfo
			{
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				nullptr, /// pNext must be NULL or a pointer to a valid instance of VkDeviceQueueGlobalPriorityCreateInfoEXT
				0u,      /// flags must be a valid combination of VkDeviceQueueCreateFlagBits values
				FamilyPropertyIndex,
				QueueFamilyProperties[FamilyPropertyIndex].queueCount,
				nullptr
			};
			QueueCreateInfos.emplace_back(QueueCreateInfo);
			/// VkDeviceQueueCreateFlagBits::VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT specifies that the device queue is a protected-capable queue. 
			/// If the protected memory feature is not enabled, the VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT bit of flags must not be set.
		}

		if (GpuType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) /// preference for discrete gpu
		{
			break;
		}
	}

	assert(GpuIndex == DiscreteGpuIndex);
	if (GpuIndex != DiscreteGpuIndex)
	{
		LOG_INFO("Can't find discrete gpu, the valid gpu type is \"{}\"", GetGpuTypeName(GpuType));
	}
	
	m_PhysicalDevice = PhysicalDevices[GpuIndex];

	std::vector<float32_t> QueuePriorities(NumQueuePriority, 1.0f);

	for (auto& QueueCreateInfo : QueueCreateInfos)
	{
		QueueCreateInfo.pQueuePriorities = QueuePriorities.data();  /// Each element of pQueuePriorities must be between 0.0 and 1.0 inclusive
	}

	std::vector<const char8_t*> EnabledLayers
	{
#if defined(_DEBUG)
#endif
	};

	std::vector<const char8_t*> EnabledExtensions
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

	VERIFY_VK(vkEnumerateDeviceLayerProperties(m_PhysicalDevice, &Count, nullptr));
	std::vector<VkLayerProperties> SupportedLayers(Count);
	VERIFY_VK(vkEnumerateDeviceLayerProperties(m_PhysicalDevice, &Count, SupportedLayers.data()));
	EnabledLayers = GetSupportedProperties<VkLayerProperties>(SupportedLayers, EnabledLayers);
	LOG_INFO("Enabled VkDevice layers: {}", GetEnabledPropertiesMessage(EnabledLayers));

	VERIFY_VK(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &Count, nullptr));
	std::vector<VkExtensionProperties> SupportedExtensions(Count);
	VERIFY_VK(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &Count, SupportedExtensions.data()));
	EnabledExtensions = GetSupportedProperties<VkExtensionProperties>(SupportedExtensions, EnabledExtensions);
	LOG_INFO("Enabled VkDevice extensions: {}", GetEnabledPropertiesMessage(EnabledExtensions));

	if (std::find_if(EnabledExtensions.begin(), EnabledExtensions.end(), [](auto Extension) {
			return std::strcmp(Extension, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0;
		}) == EnabledExtensions.end())
	{
		LOG_ERROR("The swapchain is not supported on this device.");
		assert(false);
	}

	for (auto Extension : EnabledExtensions)
	{
#if VK_EXT_debug_marker
		if (std::strcmp(Extension, VK_EXT_DEBUG_MARKER_EXTENSION_NAME) == 0)
		{
			m_Extensions.DebugMarker = true;
		}
#endif
#if VK_KHR_synchronization2
		if (std::strcmp(Extension, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME) == 0)
		{
			m_Extensions.Synchronization2 = true;
		}
#endif
#if VK_KHR_timeline_semaphore
		if (std::strcmp(Extension, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME) == 0)
		{
			m_Extensions.TimelineSemaphore = true;
		}
#endif
#if VK_KHR_create_renderpass2
		if (std::strcmp(Extension, VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME) == 0)
		{
			m_Extensions.RenderPass2 = true;
		}
#endif
#if VK_EXT_extended_dynamic_state
		if (std::strcmp(Extension, VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME) == 0)
		{
			m_Extensions.ExtendedDynamicState = true;
		}
#endif
#if VK_EXT_extended_dynamic_state2
		if (std::strcmp(Extension, VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME) == 0)
		{
			m_Extensions.ExtendedDynamicState2 = true;
		}
#endif
#if VK_KHR_maintenance1
		if (std::strcmp(Extension, VK_KHR_MAINTENANCE1_EXTENSION_NAME) == 0)
		{
			m_Extensions.Maintenance1 = true;
		}
#endif
	}

	m_Extensions.DebugReport = Instance->SupportDebugReport();
	m_Extensions.DebugUtils = Instance->SupportDebugUtils();

	VkPhysicalDeviceFeatures DeviceFeatures;
	vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &DeviceFeatures);

	VkDeviceCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0u,
		static_cast<uint32_t>(QueueCreateInfos.size()),
		QueueCreateInfos.data(),
		static_cast<uint32_t>(EnabledLayers.size()),
		EnabledLayers.data(),
		static_cast<uint32_t>(EnabledExtensions.size()),
		EnabledExtensions.data(),
		&DeviceFeatures
	};

	VERIFY_VK(vkCreateDevice(m_PhysicalDevice, &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));

#if USE_VK_LOADER
	VulkanLoader::Get().LoadDeviceFuncs(m_Handle);
#else
	VulkanLoader::Get().LoadExtFuncs(m_Instance, m_Handle, m_Extensions.DebugMarker, Instance->SupportDebugUtils());
#endif

	vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);

	VkPhysicalDeviceProperties DeviceProperties;
	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &DeviceProperties);
	m_Adapter = DeviceProperties.deviceName;

	m_PhysicalLimits = std::move(DeviceProperties.limits);
	m_PhysicalDeviceSparseProperties = std::move(DeviceProperties.sparseProperties);

	LOG_INFO("Create Vulkan device on adapter: \"{} {}\", DeviceID = {}. VulkanAPI Version: {}.{}.{}",
		GetVendorName(DeviceProperties.vendorID),
		DeviceProperties.deviceName,
		DeviceProperties.deviceID,
		VK_VERSION_MAJOR(DeviceProperties.apiVersion),
		VK_VERSION_MINOR(DeviceProperties.apiVersion),
		VK_VERSION_PATCH(DeviceProperties.apiVersion));

	assert(GraphicsQueueFamilyIndex != ~0u);
	std::array<uint32_t, static_cast<size_t>(EQueueType::Compute) + 1ull> QueueFamilyIndices
	{
		GraphicsQueueFamilyIndex, ~0u, ComputeQueueFamilyIndex
	};
	for (uint32_t QueueFamilyIndex = 0u; QueueFamilyIndex < QueueFamilyIndices.size(); ++QueueFamilyIndex)
	{
		if (QueueFamilyIndices[QueueFamilyIndex] != ~0u)
		{
			m_Queues[QueueFamilyIndex] = std::make_unique<VulkanQueue>(this, static_cast<EQueueType>(QueueFamilyIndex), QueueFamilyIndices[QueueFamilyIndex]);
		}
	}

	m_PipelineCache = std::make_unique<VulkanPipelineCache>(this);
}

void VulkanDevice::SetObjectName(uint64_t ObjectHandle, VkObjectType Type, const char8_t* Name)
{
	assert(ObjectHandle && Name && Type != VK_OBJECT_TYPE_UNKNOWN);

	if (EnabledExtensions().DebugUtils)
	{
		VkDebugUtilsObjectNameInfoEXT ObjectNameInfo
		{
			VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			nullptr,
			Type,
			ObjectHandle,
			Name
		};

		VERIFY_VK(vkSetDebugUtilsObjectNameEXT(Get(), &ObjectNameInfo));
	}
	else
	{
		if (EnabledExtensions().DebugMarker)
		{
			VkDebugMarkerObjectNameInfoEXT ObjectNameInfo
			{
				VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT,
				nullptr,
				static_cast<VkDebugReportObjectTypeEXT>(Type),
				ObjectHandle,
				Name
			};

			VERIFY_VK(vkDebugMarkerSetObjectNameEXT(Get(), &ObjectNameInfo));
		}
	}
}

VulkanDescriptor VulkanDevice::GetOrAllocateDescriptor(const GraphicsPipelineDesc& Desc)
{
	auto KeyBindings = VulkanDescriptor::MakeKeyBindings(this, Desc);

	VulkanDescriptor Descriptor;

	auto& Bindings = KeyBindings.DescriptorSetLayoutBindings();
	auto& PushConstantRanges = KeyBindings.PushConstantRanges();

	VkDescriptorSetLayoutCreateInfo DescriptorLayoutCreateInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0u,
		static_cast<uint32_t>(Bindings.size()),
		Bindings.data()
	};
	VERIFY_VK(vkCreateDescriptorSetLayout(Get(), &DescriptorLayoutCreateInfo, VK_ALLOCATION_CALLBACKS, &Descriptor.DescriptorSetLayout));

	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0u,
		1u,
		&Descriptor.DescriptorSetLayout,
		static_cast<uint32_t>(PushConstantRanges.size()),
		PushConstantRanges.data()
	};
	VERIFY_VK(vkCreatePipelineLayout(Get(), &PipelineLayoutCreateInfo, VK_ALLOCATION_CALLBACKS, &Descriptor.PipelineLayout));

	{
		if (m_DescriptorPools.empty() || m_DescriptorPools.back()->IsFull())
		{
			m_DescriptorPools.emplace_back(std::make_unique<VulkanDescriptorPool>(this));
		}
		Descriptor.DescriptorSet = m_DescriptorPools.back()->Alloc(Descriptor.DescriptorSetLayout);
	}

	return Descriptor;
}

std::shared_ptr<VulkanFence> VulkanDevice::GetOrCreateFence(bool8_t Signaled)
{
	std::lock_guard ScopedLocker(m_FenceCache.first);
	if (!m_FenceCache.second.empty())
	{
		auto Ret = m_FenceCache.second.front();
		m_FenceCache.second.pop();
		return Ret;
	}

	return std::make_shared<VulkanFence>(this, Signaled);
}

void VulkanDevice::FreeFence(std::shared_ptr<VulkanFence> Fence)
{
	if (Fence)
	{
		std::lock_guard ScopedLocker(m_FenceCache.first);
		Fence->Reset();
		m_FenceCache.second.emplace(std::move(Fence));
	}
}

void VulkanDevice::SubmitCommandBuffers(const std::vector<ICommandBufferSharedPtr>& Commands)
{
	std::array<std::vector<ICommandBufferSharedPtr>, static_cast<size_t>(EQueueType::Compute) + 1ull> QueueCommands;
	for (auto Command : Commands)
	{
		if (Command)
		{
			auto OwnerQueue = std::static_pointer_cast<VulkanCommandBuffer>(Command)->Pool()->QueueType();
			QueueCommands[static_cast<size_t>(OwnerQueue)].emplace_back(Command);
		}
	}

	for (uint32_t CommandIndex = 0u; CommandIndex < QueueCommands.size(); ++CommandIndex)
	{
		if (CommandIndex < m_Queues.size())
		{
			if (QueueCommands[CommandIndex].size() > 0u)
			{
				m_Queues[CommandIndex]->Submit(QueueCommands[CommandIndex]);
			}
		}
		else
		{
			assert(QueueCommands[CommandIndex].size() == 0u);
		}
	}
}

void VulkanDevice::SubmitCommandBuffer(ICommandBufferSharedPtr& Command)
{
	assert(Command);
	Queue(std::static_pointer_cast<VulkanCommandBuffer>(Command)->Pool()->QueueType())->Submit(std::vector<ICommandBufferSharedPtr>{Command});
}

VulkanDevice::~VulkanDevice()
{
	m_DescriptorPools.clear();
	m_PipelineCache.reset();
	m_Samplers.second.clear();
	m_FrameBufferCache.clear();
	m_GraphicsPipelineCache.clear();

	while (!m_FenceCache.second.empty())
	{
		m_FenceCache.second.pop();
	}

	for (auto& queue : m_Queues)
	{
		queue.reset();
	}

	vkDestroyDevice(Get(), VK_ALLOCATION_CALLBACKS);

	LOG_DEBUG("Vulkan device destroyed.");
}

NAMESPACE_END(RHI)

#endif

#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanInstance.h"
#include "RHI/Vulkan/VulkanRHI.h"

VulkanDevice::VulkanDevice()
{
	auto LayerAndExtensionsConfigs = VulkanLayerExtensionConfigurations::Load(VK_LAYER_EXT_CONFIG_NAME);

	m_Instance = std::make_unique<VulkanInstance>(LayerAndExtensionsConfigs.get());

	uint32_t GraphicsQueueIndex = ~0u, ComputeQueueIndex = ~0u, TransferQueueIndex = ~0u, PresentQueueIndex = ~0u;

	std::vector<const vk::PhysicalDevice*> DiscreteGpus;
	std::vector<const vk::PhysicalDevice*> OtherGpus;

	auto PhysicalDevices = m_Instance->Get().enumeratePhysicalDevices();
	for (const auto& PhysicalDevice : PhysicalDevices)
	{
		if (GetQueueFamilyIndex(&PhysicalDevice, GraphicsQueueIndex, ComputeQueueIndex, TransferQueueIndex, PresentQueueIndex))
		{
			if (PhysicalDevice.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			{
				DiscreteGpus.push_back(&PhysicalDevice);
			}
			else
			{
				OtherGpus.push_back(&PhysicalDevice);
			}
		}
	}

	if (DiscreteGpus.size() == 0u && OtherGpus.size() > 0u)
	{
		LOG_WARNING("VulkanRHI: Can't find discrete GPU");
	}

	m_PhysicalDevice = DiscreteGpus.size() > 0u ? *DiscreteGpus[0] : (OtherGpus.size() > 0u ? *OtherGpus[0] : vk::PhysicalDevice{});
	assert(m_PhysicalDevice);

	GetQueueFamilyIndex(&m_PhysicalDevice, GraphicsQueueIndex, ComputeQueueIndex, TransferQueueIndex, PresentQueueIndex);

	auto WantedLayers = VulkanLayer::GetWantedDeviceLayers();
	auto WantedExtensions = VulkanExtension::GetWantedDeviceExtensions();

	std::vector<const char8_t*> EnabledLayers;
	std::vector<const char8_t*> EnabledExtensions;

	LOG_DEBUG("VulkanRHI: Found supported device layers:");
	auto LayerProperties = m_PhysicalDevice.enumerateDeviceLayerProperties();
	for each (const auto& LayerProperty in LayerProperties)
	{
		auto LayerIt = std::find_if(WantedLayers.begin(), WantedLayers.end(), [&LayerProperty](const std::unique_ptr<VulkanLayer>& Layer) {
			return strcmp(Layer->GetName(), LayerProperty.layerName.data()) == 0;
			});
		if (LayerIt != WantedLayers.end())
		{
			EnabledLayers.push_back(LayerProperty.layerName.data());
			(*LayerIt)->SetEnabled(LayerAndExtensionsConfigs.get(), true);
		}

		LOG_DEBUG("\t\t\t\t\"{}\"", LayerProperty.layerName.data());
	}

	LOG_DEBUG("VulkanRHI: Found supported device extensions:");
	auto ExtensionProperties = m_PhysicalDevice.enumerateDeviceExtensionProperties();
	for each (const auto& ExtensionProperty in ExtensionProperties)
	{
		auto ExtensionIt = std::find_if(WantedExtensions.begin(), WantedExtensions.end(), [&ExtensionProperty](const std::unique_ptr<VulkanExtension>& Extension) {
			return strcmp(Extension->GetName(), ExtensionProperty.extensionName.data()) == 0;
			});
		if (ExtensionIt != WantedExtensions.end())
		{
			EnabledExtensions.push_back(ExtensionProperty.extensionName.data());
			(*ExtensionIt)->SetEnabled(LayerAndExtensionsConfigs.get(), true);
		}

		LOG_DEBUG("\t\t\t\t\"{}\"", ExtensionProperty.extensionName.data());
	}

	LogEnableLayerAndExtensions(WantedLayers, WantedExtensions, "device");

	std::set<uint32_t> QueueFamilyIndices{ GraphicsQueueIndex, PresentQueueIndex };
	if (VulkanRHI::GetGraphicsSettings()->EnableComputeQueue)
	{
		QueueFamilyIndices.insert(ComputeQueueIndex);
	}
	if (VulkanRHI::GetGraphicsSettings()->EnableTransferQueue)
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
		.setQueueCreateInfos(QueueCreateInfos)
		.setPEnabledLayerNames(EnabledLayers)
		.setPEnabledExtensionNames(EnabledExtensions)
		.setPEnabledFeatures(&DeviceFeatures);

	for (auto& Extension : WantedExtensions)
	{
		if (Extension->IsEnabled())
		{
			Cast<VulkanDeviceExtension>(Extension)->PreDeviceCreation(LayerAndExtensionsConfigs.get(), CreateInfo);
		}
	}

	VERIFY_VK(m_PhysicalDevice.createDevice(&CreateInfo, nullptr, &m_LogicalDevice));

	auto PhysicalDeviceProperties = m_PhysicalDevice.getProperties();
	LOG_INFO("VulkanRHI: Created vulkan device on adapter: \"{}\", DeviceID = {}. DeviceType = {}. VulkanAPI Version: {}.{}.{}",
		PhysicalDeviceProperties.deviceName.data(),
		PhysicalDeviceProperties.deviceID,
		vk::to_string(PhysicalDeviceProperties.deviceType).c_str(),
		VK_VERSION_MAJOR(PhysicalDeviceProperties.apiVersion),
		VK_VERSION_MINOR(PhysicalDeviceProperties.apiVersion),
		VK_VERSION_PATCH(PhysicalDeviceProperties.apiVersion));

#if USE_DYNAMIC_VK_LOADER
	VULKAN_HPP_DEFAULT_DISPATCHER.init(m_LogicalDevice);
#endif
}

void VulkanDevice::WaitIdle()
{
}

bool8_t VulkanDevice::GetQueueFamilyIndex(
	const vk::PhysicalDevice* PhysicalDevice, 
	uint32_t& GraphicsQueueIndex, 
	uint32_t& ComputeQueueIndex, 
	uint32_t& TransferQueueIndex, 
	uint32_t& PresentQueueIndex) const
{
#define IS_VALID_PROPERTY_INDEX(Index) Property.queueCount > 0u && Index == std::numeric_limits<uint32_t>().max()
#define IS_VALID_QUEUE_INDEX(Index) Index != std::numeric_limits<uint32_t>().max()
#define IS_VALID_QUEUE_FLAGS(Flag) (Property.queueFlags & vk::QueueFlagBits::Flag) == vk::QueueFlagBits::Flag
	
	GraphicsQueueIndex = ComputeQueueIndex = TransferQueueIndex = std::numeric_limits<uint32_t>().max();

	auto Properties = PhysicalDevice->getQueueFamilyProperties();
	for (uint32_t Index = 0u; Index < Properties.size(); ++Index)
	{
		const auto& Property = Properties[Index];
		if (IS_VALID_PROPERTY_INDEX(GraphicsQueueIndex) && IS_VALID_QUEUE_FLAGS(eGraphics))
		{
			GraphicsQueueIndex = Index;
		}
		if (IS_VALID_PROPERTY_INDEX(ComputeQueueIndex) && IS_VALID_QUEUE_FLAGS(eCompute) && !(IS_VALID_QUEUE_FLAGS(eGraphics)))
		{
			ComputeQueueIndex = Index;
		}
		if (IS_VALID_PROPERTY_INDEX(TransferQueueIndex) && IS_VALID_QUEUE_FLAGS(eTransfer) && !(IS_VALID_QUEUE_FLAGS(eGraphics)) && !(IS_VALID_QUEUE_FLAGS(eCompute)))
		{
			TransferQueueIndex = Index;
		}
		if (IS_VALID_PROPERTY_INDEX(PresentQueueIndex))
		{
#if PLATFORM_WIN32
			if (PhysicalDevice->getWin32PresentationSupportKHR(Index))
			{
				PresentQueueIndex = Index;
			}
#endif
		}
	}

	return IS_VALID_QUEUE_INDEX(GraphicsQueueIndex) && IS_VALID_QUEUE_INDEX(ComputeQueueIndex) && IS_VALID_QUEUE_INDEX(TransferQueueIndex) && IS_VALID_QUEUE_INDEX(PresentQueueIndex);
#undef IS_VALID
#undef IS_INVALID
#undef IS_FLAGS_MATCH
}

RHIShaderPtr VulkanDevice::CreateShader(const RHIShaderCreateInfo& /*RHICreateInfo*/)
{
	return RHIShaderPtr();
}

RHIImagePtr VulkanDevice::CreateImage(const RHIImageCreateInfo& /*RHICreateInfo*/)
{
	return RHIImagePtr();
}

RHIInputLayoutPtr VulkanDevice::CreateInputLayout(const RHIInputLayoutCreateInfo& /*RHICreateInfo*/)
{
	return RHIInputLayoutPtr();
}

RHIFrameBufferPtr VulkanDevice::CreateFrameBuffer(const RHIFrameBufferCreateInfo& /*RHICreateInfo*/)
{
	return RHIFrameBufferPtr();
}

RHIGraphicsPipelinePtr VulkanDevice::CreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& /*RHICreateInfo*/)
{
	return RHIGraphicsPipelinePtr();
}

RHIBufferPtr VulkanDevice::CreateBuffer(const RHIBufferCreateInfo& /*RHICreateInfo*/)
{
	return RHIBufferPtr();
}

RHISamplerPtr VulkanDevice::CreateSampler(const RHISamplerCreateInfo& /*RHICreateInfo*/)
{
	return RHISamplerPtr();
}

RHICommandBufferPoolPtr VulkanDevice::CreateCommandBufferPool(ERHIDeviceQueueType /*QueueType*/)
{
	return RHICommandBufferPoolPtr();
}

void VulkanDevice::SubmitCommandBuffers(ERHIDeviceQueueType /*QueueType*/, const std::vector<RHICommandBuffer*>& /*Commands*/)
{
}

void VulkanDevice::SubmitCommandBuffer(ERHIDeviceQueueType /*QueueType*/, RHICommandBuffer* /*Command*/)
{
}

void VulkanDevice::SubmitCommandBuffers(const std::vector<RHICommandBuffer*>& /*Commands*/)
{
}

void VulkanDevice::SubmitCommandBuffer(RHICommandBuffer* /*Command*/)
{
}

RHICommandBufferPtr VulkanDevice::GetOrAllocateCommandBuffer(ERHIDeviceQueueType /*QueueType*/, ERHICommandBufferLevel /*Level*/, bool8_t /*AutoBegin*/)
{
	return RHICommandBufferPtr();
}

VulkanDevice::~VulkanDevice()
{
	m_LogicalDevice.destroy();
	m_LogicalDevice = nullptr;
}