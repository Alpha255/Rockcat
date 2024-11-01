#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanInstance.h"
#include "RHI/Vulkan/VulkanRHI.h"
#include "RHI/Vulkan/VulkanQueue.h"
#include "RHI/Vulkan/VulkanCommandPool.h"
#include "RHI/Vulkan/VulkanLayerExtensions.h"
#include "RHI/Vulkan/VulkanShader.h"
#include "Engine/Services/TaskFlowService.h"

VulkanDevice::VulkanDevice(VulkanLayerExtensionConfigurations* Configs)
{
	m_Instance = std::make_unique<VulkanInstance>(Configs);

	auto GetQueueFamilyIndex = [](
		const vk::PhysicalDevice& PhysicalDevice, 
		uint32_t& GraphicsQueueIndex,
		uint32_t& ComputeQueueIndex,
		uint32_t& TransferQueueIndex,
		uint32_t& PresentQueueIndex) {
#define IS_VALID_PROPERTY_INDEX(Index) Property.queueCount > 0u && Index == std::numeric_limits<uint32_t>::max()
#define IS_VALID_QUEUE_INDEX(Index) Index != std::numeric_limits<uint32_t>::max()
#define IS_VALID_QUEUE_FLAGS(Flag) (Property.queueFlags & vk::QueueFlagBits::Flag) == vk::QueueFlagBits::Flag

			GraphicsQueueIndex = ComputeQueueIndex = TransferQueueIndex = std::numeric_limits<uint32_t>::max();

			auto Properties = PhysicalDevice.getQueueFamilyProperties();
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
					if (PhysicalDevice.getWin32PresentationSupportKHR(Index))
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
	};

	std::vector<const vk::PhysicalDevice*> DiscreteGpus;
	std::vector<const vk::PhysicalDevice*> OtherGpus;
	uint32_t GraphicsQueueIndex = ~0u, ComputeQueueIndex = ~0u, TransferQueueIndex = ~0u, PresentQueueIndex = ~0u;

	auto PhysicalDevices = m_Instance->GetNative().enumeratePhysicalDevices();
	for (const auto& PhysicalDevice : PhysicalDevices)
	{
		if (GetQueueFamilyIndex(PhysicalDevice, GraphicsQueueIndex, ComputeQueueIndex, TransferQueueIndex, PresentQueueIndex))
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

	GetQueueFamilyIndex(m_PhysicalDevice, GraphicsQueueIndex, ComputeQueueIndex, TransferQueueIndex, PresentQueueIndex);

	auto WantedLayers = VulkanLayer::GetWantedDeviceLayers();
	auto WantedExtensions = VulkanExtension::GetWantedDeviceExtensions();

	std::vector<const char*> EnabledLayers;
	std::vector<const char*> EnabledExtensions;

	LOG_DEBUG("VulkanRHI: Found valid device layers:");
	auto LayerProperties = m_PhysicalDevice.enumerateDeviceLayerProperties();
	for (const auto& LayerProperty : LayerProperties)
	{
		auto LayerIt = std::find_if(WantedLayers.begin(), WantedLayers.end(), [&LayerProperty](const std::unique_ptr<VulkanLayer>& Layer) {
			return strcmp(Layer->GetName(), LayerProperty.layerName.data()) == 0;
			});
		if (LayerIt != WantedLayers.end())
		{
			if ((*LayerIt)->SetEnabled(Configs, true))
			{
				EnabledLayers.push_back(LayerProperty.layerName.data());
			}
		}

		LOG_DEBUG("\t\t\t\t\"{}\"", LayerProperty.layerName.data());
	}

	LOG_DEBUG("VulkanRHI: Found valid device extensions:");
	auto ExtensionProperties = m_PhysicalDevice.enumerateDeviceExtensionProperties();
	for (const auto& ExtensionProperty : ExtensionProperties)
	{
		auto ExtensionIt = std::find_if(WantedExtensions.begin(), WantedExtensions.end(), [&ExtensionProperty](const std::unique_ptr<VulkanExtension>& Extension) {
			return strcmp(Extension->GetName(), ExtensionProperty.extensionName.data()) == 0;
			});
		if (ExtensionIt != WantedExtensions.end())
		{
			if ((*ExtensionIt)->SetEnabled(Configs, true))
			{
				EnabledExtensions.push_back(ExtensionProperty.extensionName.data());
			}
		}

		LOG_DEBUG("\t\t\t\t\"{}\"", ExtensionProperty.extensionName.data());
	}

	LogEnabledLayerAndExtensions(WantedLayers, WantedExtensions, "device");

	std::set<uint32_t> QueueFamilyIndices{ GraphicsQueueIndex, PresentQueueIndex };
	if (VulkanRHI::GetGraphicsSettings().EnableAsyncCompute)
	{
		if (ComputeQueueIndex != GraphicsQueueIndex)
		{
			QueueFamilyIndices.insert(ComputeQueueIndex);
		}
		else
		{
			const_cast<GraphicsSettings*>(&VulkanRHI::GetGraphicsSettings())->EnableAsyncCompute = false;
		}
	}
	if (VulkanRHI::GetGraphicsSettings().EnableAsyncTransfer)
	{
		if (TransferQueueIndex != GraphicsQueueIndex)
		{
			QueueFamilyIndices.insert(TransferQueueIndex);
		}
		else
		{
			const_cast<GraphicsSettings*>(&VulkanRHI::GetGraphicsSettings())->EnableAsyncTransfer = false;
		}
	}

	const float Priority = 1.0f;
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
			Cast<VulkanDeviceExtension>(Extension.get())->PreDeviceCreation(Configs, CreateInfo);
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

	m_Limits = PhysicalDeviceProperties.limits;

#if USE_DYNAMIC_VK_LOADER
	VULKAN_HPP_DEFAULT_DISPATCHER.init(m_LogicalDevice);
#endif

	m_Queues[(size_t)ERHIDeviceQueue::Graphics] = std::move(std::make_unique<VulkanQueue>(*this, ERHIDeviceQueue::Graphics, GraphicsQueueIndex));
	if (VulkanRHI::GetGraphicsSettings().EnableAsyncTransfer)
	{
		m_Queues[(size_t)ERHIDeviceQueue::Transfer] = std::move(std::make_unique<VulkanQueue>(*this, ERHIDeviceQueue::Transfer, TransferQueueIndex));
	}
	if (VulkanRHI::GetGraphicsSettings().EnableAsyncCompute)
	{
		m_Queues[(size_t)ERHIDeviceQueue::Compute] = std::move(std::make_unique<VulkanQueue>(*this, ERHIDeviceQueue::Compute, ComputeQueueIndex));
	}

	if (PresentQueueIndex != GraphicsQueueIndex)
	{
		assert(false);
	}

	//m_TransferCmdMgr = std::move(std::make_unique<VulkanCommandBufferManager>(*this, *m_Queues[(size_t)ERHIDeviceQueue::Transfer]));
	//m_ComputeCmdMgr = std::move(std::make_unique<VulkanCommandBufferManager>(*this, *m_Queues[(size_t)ERHIDeviceQueue::Compute]));
	//for (uint8_t Index = 0u; Index < TaskFlowService::Get().GetNumWorkThreads(); ++Index)
	//{
	//	m_GraphicsCmdMgrs.emplace(std::make_shared<VulkanCommandBufferManager>(*this, *m_Queues[(size_t)ERHIDeviceQueue::Graphics]));
	//}
}

RHIShaderPtr VulkanDevice::CreateShader(const RHIShaderCreateInfo& CreateInfo)
{
	return std::make_shared<VulkanShader>(*this, CreateInfo);
}

RHITexturePtr VulkanDevice::CreateTexture(const RHITextureCreateInfo& /*CreateInfo*/)
{
	return RHITexturePtr();
}

RHIInputLayoutPtr VulkanDevice::CreateInputLayout(const RHIInputLayoutCreateInfo& /*CreateInfo*/)
{
	return RHIInputLayoutPtr();
}

RHIFrameBufferPtr VulkanDevice::CreateFrameBuffer(const RHIFrameBufferCreateInfo& /*CreateInfo*/)
{
	return RHIFrameBufferPtr();
}

RHIGraphicsPipelinePtr VulkanDevice::CreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& /*CreateInfo*/)
{
	return RHIGraphicsPipelinePtr();
}

RHIBufferPtr VulkanDevice::CreateBuffer(const RHIBufferCreateInfo& /*CreateInfo*/)
{
	return RHIBufferPtr();
}

RHISamplerPtr VulkanDevice::CreateSampler(const RHISamplerCreateInfo& /*CreateInfo*/)
{
	return RHISamplerPtr();
}

const vk::Instance& VulkanDevice::GetInstance() const
{
	return m_Instance->GetNative();
}

VulkanDevice::~VulkanDevice()
{
	m_LogicalDevice.destroy();
	m_LogicalDevice = nullptr;
}