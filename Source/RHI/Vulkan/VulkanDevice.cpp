#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanInstance.h"
#include "RHI/Vulkan/VulkanRHI.h"
#include "RHI/Vulkan/VulkanQueue.h"
#include "RHI/Vulkan/VulkanCommandPool.h"
#include "RHI/Vulkan/VulkanLayerExtensions.h"
#include "RHI/Vulkan/VulkanShader.h"
#include "RHI/Vulkan/VulkanPipeline.h"
#include "RHI/Vulkan/VulkanCommandListContext.h"
#include "RHI/Vulkan/VulkanDescriptor.h"
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
		LOG_CAT_WARNING(LogVulkanRHI, "Can't find discrete GPU");
	}

	m_PhysicalDevice = DiscreteGpus.size() > 0u ? *DiscreteGpus[0] : (OtherGpus.size() > 0u ? *OtherGpus[0] : vk::PhysicalDevice{});
	assert(m_PhysicalDevice);

	GetQueueFamilyIndex(m_PhysicalDevice, GraphicsQueueIndex, ComputeQueueIndex, TransferQueueIndex, PresentQueueIndex);

	auto WantedLayers = VulkanLayer::GetWantedDeviceLayers(*Configs);
	auto WantedExtensions = VulkanExtension::GetWantedDeviceExtensions(*Configs);

	std::vector<const char*> EnabledLayers;
	std::vector<const char*> EnabledExtensions;

	std::string LogValidDeviceLayers("Found available device layers:\n");
	auto LayerProperties = m_PhysicalDevice.enumerateDeviceLayerProperties();

	for (const auto& LayerProperty : LayerProperties)
	{
		LogValidDeviceLayers += StringUtils::Format("\t\t\t\t\"%s\"\n", LayerProperty.layerName.data());
	}
	LOG_CAT_DEBUG(LogVulkanRHI, LogValidDeviceLayers.c_str());

	for (auto& Layer : WantedLayers)
	{
		auto LayerIt = std::find_if(LayerProperties.cbegin(), LayerProperties.cend(), [&Layer](const vk::LayerProperties& Property) {
			return strcmp(Layer->GetName(), Property.layerName.data()) == 0;
		});

		Layer->SetEnabledInConfig(LayerIt != LayerProperties.cend());

		if (Layer->IsSupported() && Layer->IsEnabled())
		{
			EnabledLayers.push_back(Layer->GetName());
		}
	}

	std::string LogValidDeviceExtensions("Found available device extensions:\n");
	auto ExtensionProperties = m_PhysicalDevice.enumerateDeviceExtensionProperties();

	for (const auto& ExtensionProperty : ExtensionProperties)
	{
		LogValidDeviceExtensions += StringUtils::Format("\t\t\t\t\"%s\"\n", ExtensionProperty.extensionName.data());
	}
	LOG_CAT_DEBUG(LogVulkanRHI, LogValidDeviceExtensions.c_str());

	for (auto& Ext : WantedExtensions)
	{
		auto ExtensionIt = std::find_if(ExtensionProperties.cbegin(), ExtensionProperties.cend(), [&Ext](const vk::ExtensionProperties& Property) {
			return strcmp(Ext->GetName(), Property.extensionName.data()) == 0;
		});

		Ext->SetEnabledInConfig(ExtensionIt != ExtensionProperties.cend());

		if (Ext->IsSupported() && Ext->IsEnabled())
		{
			Ext->SetSpecVersion(ExtensionIt->specVersion);
			EnabledExtensions.push_back(Ext->GetName());
		}
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
		QueueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo())
			.setQueueFamilyIndex(QueueFamilyIndex)
			.setQueueCount(1u)
			.setPQueuePriorities(&Priority);
	}

	auto DeviceFeatures = m_PhysicalDevice.getFeatures();

	vk::DeviceCreateInfo CreateInfo;
	CreateInfo.setQueueCreateInfos(QueueCreateInfos)
		.setPEnabledLayerNames(EnabledLayers)
		.setPEnabledExtensionNames(EnabledExtensions)
		.setPEnabledFeatures(&DeviceFeatures);

	for (auto& Extension : WantedExtensions)
	{
		if (Extension->IsEnabled() && Extension->GetOnDeviceCreation())
		{
			Extension->GetOnDeviceCreation()(*Configs, CreateInfo);
		}
	}

	VERIFY_VK(m_PhysicalDevice.createDevice(&CreateInfo, nullptr, &m_LogicalDevice));

	auto PhysicalDeviceProperties = m_PhysicalDevice.getProperties();
	LOG_CAT_INFO(LogVulkanRHI, "Created vulkan device on adapter: \"{}\", DeviceID = {}. DeviceType = {}. VulkanAPI Version: {}.{}.{}, Driver Version: {}.{}.{}",
		PhysicalDeviceProperties.deviceName.data(),
		PhysicalDeviceProperties.deviceID,
		vk::to_string(PhysicalDeviceProperties.deviceType).c_str(),
		VK_VERSION_MAJOR(PhysicalDeviceProperties.apiVersion),
		VK_VERSION_MINOR(PhysicalDeviceProperties.apiVersion),
		VK_VERSION_PATCH(PhysicalDeviceProperties.apiVersion),
		VK_VERSION_MAJOR(PhysicalDeviceProperties.driverVersion),
		VK_VERSION_MINOR(PhysicalDeviceProperties.driverVersion),
		VK_VERSION_PATCH(PhysicalDeviceProperties.driverVersion));

	m_Limits = PhysicalDeviceProperties.limits;

#if USE_DYNAMIC_VK_LOADER
	VULKAN_HPP_DEFAULT_DISPATCHER.init(m_LogicalDevice);
#endif

	auto CreateQueueAndImmdiateCmdListContext = [this](ERHIDeviceQueue Queue, uint32_t QueueFamilyIndex, bool Enabled) {
		auto GraphicsQueueIndex = ERHIDeviceQueue::Graphics;

		if (Enabled)
		{
			m_Queues[Queue].reset(new VulkanQueue(*this, Queue, QueueFamilyIndex));
			m_ImmediateCmdListContexts[Queue] = std::make_shared<VulkanCommandListContext>(*this, *m_Queues[Queue]);
		}
		else
		{
			m_ImmediateCmdListContexts[Queue] = m_ImmediateCmdListContexts[GraphicsQueueIndex];
		}
	};

	CreateQueueAndImmdiateCmdListContext(ERHIDeviceQueue::Graphics, GraphicsQueueIndex, true);
	CreateQueueAndImmdiateCmdListContext(ERHIDeviceQueue::Transfer, TransferQueueIndex, VulkanRHI::GetGraphicsSettings().EnableAsyncTransfer);
	CreateQueueAndImmdiateCmdListContext(ERHIDeviceQueue::Compute, ComputeQueueIndex, VulkanRHI::GetGraphicsSettings().EnableAsyncCompute);
	
	assert(PresentQueueIndex == GraphicsQueueIndex);

	for (uint8_t Index = 0u; Index < TaskFlowService::Get().GetNumWorkThreads(); ++Index)
	{
		m_ThreadedCmdListContexts.emplace(std::make_shared<VulkanCommandListContext>(*this, *m_Queues[ERHIDeviceQueue::Graphics]));
	}

	m_PipelineCache = std::make_shared<VulkanPipelineCache>(*this);
}

RHIShaderPtr VulkanDevice::CreateShader(const RHIShaderCreateInfo& CreateInfo)
{
	return std::make_shared<VulkanShader>(*this, CreateInfo);
}

RHITexturePtr VulkanDevice::CreateTexture(const RHITextureCreateInfo& CreateInfo)
{
	return std::make_shared<VulkanTexture>(*this, CreateInfo);
}

RHIInputLayoutPtr VulkanDevice::CreateInputLayout(const RHIInputLayoutCreateInfo& CreateInfo)
{
	return std::make_shared<VulkanInputLayout>(CreateInfo);
}

RHIFrameBufferPtr VulkanDevice::CreateFrameBuffer(const RHIFrameBufferCreateInfo& /*CreateInfo*/)
{
	return RHIFrameBufferPtr();
}

RHIGraphicsPipelinePtr VulkanDevice::CreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& CreateInfo)
{
	return std::make_shared<VulkanGraphicsPipeline>(*this, m_PipelineCache->GetNative(), CreateInfo);
}

RHIPipelineStatePtr VulkanDevice::CreatePipelineState(const RHIGraphicsPipelineCreateInfo& CreateInfo)
{
	return std::make_shared<VulkanPipelineState>(*this, CreateInfo);
}

RHIBufferPtr VulkanDevice::CreateBuffer(const RHIBufferCreateInfo& CreateInfo)
{
	return std::make_shared<VulkanBuffer>(*this, CreateInfo);
}

RHISamplerPtr VulkanDevice::CreateSampler(const RHISamplerCreateInfo& CreateInfo)
{
	return std::make_shared<VulkanSampler>(*this, CreateInfo);
}

RHICommandListContext* VulkanDevice::GetImmediateCommandListContext(ERHIDeviceQueue Queue)
{
	return m_ImmediateCmdListContexts[Queue].get();
}

RHICommandListContextPtr VulkanDevice::AcquireDeferredCommandListContext()
{
	std::lock_guard Locker(m_CmdListContextLock);
	
	if (m_ThreadedCmdListContexts.empty())
	{
		return std::make_shared<VulkanCommandListContext>(*this, *m_Queues[ERHIDeviceQueue::Graphics]);
	}

	auto CommandListContext = m_ThreadedCmdListContexts.front();
	m_ThreadedCmdListContexts.pop();
	return CommandListContext;
}

void VulkanDevice::ReleaseDeferredCommandListContext(RHICommandListContextPtr& CmdListContext)
{
	std::lock_guard Locker(m_CmdListContextLock);

	m_ThreadedCmdListContexts.emplace(Cast<VulkanCommandListContext>(std::move(CmdListContext)));
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