#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanInstance.h"
#include "RHI/Vulkan/VulkanQueue.h"
#include "RHI/Vulkan/VulkanCommandPool.h"
#include "RHI/Vulkan/VulkanLayerExtensions.h"
#include "RHI/Vulkan/VulkanShader.h"
#include "RHI/Vulkan/VulkanPipeline.h"
#include "RHI/Vulkan/VulkanCommandListContext.h"
#include "RHI/Vulkan/VulkanDescriptor.h"
#include "RHI/Vulkan/VulkanDevelopSettings.h"
#include "RHI/Vulkan/VulkanSwapchain.h"
#include "RHI/Vulkan/VulkanMemoryAllocator.h"
#include "RHI/Vulkan/VulkanRenderPass.h"
#include "Paths.h"
#include "Services/TaskFlowService.h"
#include "Services/ShaderLibrary.h"

#if USE_DYNAMIC_VK_LOADER
	VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

VulkanDevice::VulkanDevice()
{
#if USE_DYNAMIC_VK_LOADER
	const PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_DynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	assert(vkGetInstanceProcAddr);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
#endif

	m_DevelopSettings = VulkanDevelopSettings::Load(Paths::ConfigPath() / "VkDevelopSettings.json");

	m_Instance = std::make_unique<VulkanInstance>(m_DevelopSettings->Extensions, m_DevelopSettings->DebugLayerLevel);

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
#else
	#error "Not implemented"
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
		LOG_WARNING_CAT(LogVulkanRHI, "Can't find discrete GPU");
	}

	m_PhysicalDevice = DiscreteGpus.size() > 0u ? *DiscreteGpus[0] : (OtherGpus.size() > 0u ? *OtherGpus[0] : vk::PhysicalDevice{});
	assert(m_PhysicalDevice);

	GetQueueFamilyIndex(m_PhysicalDevice, GraphicsQueueIndex, ComputeQueueIndex, TransferQueueIndex, PresentQueueIndex);

	auto WantedLayers = VulkanLayer::GetWantedDeviceLayers(m_DevelopSettings->Extensions);
	auto WantedExtensions = VulkanExtension::GetWantedDeviceExtensions(m_DevelopSettings->Extensions);

	std::vector<const char*> EnabledLayers;
	std::vector<const char*> EnabledExtensions;

	std::string LogValidDeviceLayers("Found available device layers:\n");
	auto LayerProperties = m_PhysicalDevice.enumerateDeviceLayerProperties();

	for (const auto& LayerProperty : LayerProperties)
	{
		LogValidDeviceLayers += StringUtils::Format("\t\t\t\t\"%s\"\n", LayerProperty.layerName.data());
	}
	LOG_DEBUG_CAT(LogVulkanRHI, LogValidDeviceLayers.c_str());

	for (auto& Layer : WantedLayers)
	{
		auto LayerIt = std::find_if(LayerProperties.cbegin(), LayerProperties.cend(), [&Layer](const vk::LayerProperties& Property) {
			return strcmp(Layer->GetName(), Property.layerName.data()) == 0;
		});

		if (Layer->IsEnabled())
		{
			Layer->SetEnabledInConfig(LayerIt != LayerProperties.cend());
		}

		if (Layer->IsEnabled())
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
	LOG_DEBUG_CAT(LogVulkanRHI, LogValidDeviceExtensions.c_str());

	for (auto& Ext : WantedExtensions)
	{
		auto ExtensionIt = std::find_if(ExtensionProperties.cbegin(), ExtensionProperties.cend(), [&Ext](const vk::ExtensionProperties& Property) {
			return strcmp(Ext->GetName(), Property.extensionName.data()) == 0;
		});

		if (Ext->IsEnabled())
		{
			Ext->SetEnabledInConfig(ExtensionIt != ExtensionProperties.cend());
		}

		if (Ext->IsEnabled())
		{
			Ext->SetSpecVersion(ExtensionIt->specVersion);
			EnabledExtensions.push_back(Ext->GetName());
		}
	}

	LogEnabledLayerAndExtensions(WantedLayers, WantedExtensions, "device");

	std::set<uint32_t> QueueFamilyIndices{ GraphicsQueueIndex, PresentQueueIndex };
	
	if (ComputeQueueIndex != GraphicsQueueIndex)
	{
		QueueFamilyIndices.insert(ComputeQueueIndex);
		m_Capabilities.SupportsAsyncCompute = true;
	}

	if (TransferQueueIndex != GraphicsQueueIndex)
	{
		QueueFamilyIndices.insert(TransferQueueIndex);
		m_Capabilities.SupportsTransferQueue = true;
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
			Extension->GetOnDeviceCreation()(m_DevelopSettings->Extensions, CreateInfo);
		}
	}

	VERIFY_VK(m_PhysicalDevice.createDevice(&CreateInfo, nullptr, &m_LogicalDevice));

	auto PhysicalDeviceProperties = m_PhysicalDevice.getProperties();
	LOG_INFO_CAT(LogVulkanRHI, "Created vulkan device on adapter: \"{}\", DeviceID = {}. DeviceType = {}. VulkanAPI @{}.{}.{}, Driver @{}.{}.{}",
		PhysicalDeviceProperties.deviceName.data(),
		PhysicalDeviceProperties.deviceID,
		vk::to_string(PhysicalDeviceProperties.deviceType).c_str(),
		VK_VERSION_MAJOR(PhysicalDeviceProperties.apiVersion),
		VK_VERSION_MINOR(PhysicalDeviceProperties.apiVersion),
		VK_VERSION_PATCH(PhysicalDeviceProperties.apiVersion),
		VK_VERSION_MAJOR(PhysicalDeviceProperties.driverVersion),
		VK_VERSION_MINOR(PhysicalDeviceProperties.driverVersion),
		VK_VERSION_PATCH(PhysicalDeviceProperties.driverVersion));

	m_PhysicalDeviceLimits = PhysicalDeviceProperties.limits;
	SetupCapabilities();

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
	CreateQueueAndImmdiateCmdListContext(ERHIDeviceQueue::Transfer, TransferQueueIndex, m_Capabilities.SupportsTransferQueue);
	CreateQueueAndImmdiateCmdListContext(ERHIDeviceQueue::Compute, ComputeQueueIndex, m_Capabilities.SupportsAsyncCompute);
	
	assert(PresentQueueIndex == GraphicsQueueIndex);

	for (uint8_t Index = 0u; Index < TaskFlow::Get().GetNumWorkerThreads(); ++Index)
	{
		m_ThreadedCmdListContexts.emplace(std::make_shared<VulkanCommandListContext>(*this, *m_Queues[ERHIDeviceQueue::Graphics]));
	}

	m_PipelineCache = std::make_shared<VulkanPipelineCache>(*this);
	
	VulkanMemoryAllocator::Create(*this);

	ShaderLibrary::Get().RegisterActiveCompiler(GetType());
}

RHIShaderPtr VulkanDevice::CreateShader(const RHIShaderDesc& Desc) const
{
	return std::make_shared<VulkanShader>(*this, Desc);
}

RHITexturePtr VulkanDevice::CreateTexture(const RHITextureDesc& Desc) const
{
	return std::make_shared<VulkanTexture>(*this, Desc);
}

RHIInputLayoutPtr VulkanDevice::CreateInputLayout(const RHIInputLayoutDesc& Desc) const
{
	return std::make_shared<VulkanInputLayout>(Desc);
}

RHIFrameBufferPtr VulkanDevice::CreateFrameBuffer(const RHIFrameBufferDesc& Desc) const
{
	return std::make_shared<VulkanFrameBuffer>(*this, vk::RenderPass{}, Desc);
}

RHIGraphicsPipelinePtr VulkanDevice::CreateGraphicsPipeline(const RHIGraphicsPipelineDesc& Desc) const
{
	return std::make_shared<VulkanGraphicsPipeline>(*this, m_PipelineCache->GetNative(), Desc);
}

RHIPipelineStatePtr VulkanDevice::CreatePipelineState(const RHIGraphicsPipelineDesc& Desc) const
{
	return std::make_shared<VulkanPipelineState>(*this, Desc);
}

RHIBufferPtr VulkanDevice::CreateBuffer(const RHIBufferDesc& Desc) const
{
	return std::make_shared<VulkanBuffer>(*this, Desc);
}

RHISamplerPtr VulkanDevice::CreateSampler(const RHISamplerDesc& Desc) const
{
	return std::make_shared<VulkanSampler>(*this, Desc);
}

RHISwapchainPtr VulkanDevice::CreateSwapchain(const RHISwapchainDesc& Desc) const
{
	return std::make_shared<VulkanSwapchain>(*this, Desc);
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

const VulkanQueue& VulkanDevice::GetQueue(ERHIDeviceQueue Queue) const
{
	assert(Queue < ERHIDeviceQueue::Num);
	return *m_Queues[Queue];
}

void VulkanDevice::SetupCapabilities()
{
	m_Capabilities.MaxTextureDimension1D = m_PhysicalDeviceLimits.maxImageDimension1D;
	m_Capabilities.MaxTextureDimension2D = m_PhysicalDeviceLimits.maxImageDimension2D;
	m_Capabilities.MaxTextureDimension3D = m_PhysicalDeviceLimits.maxImageDimension3D;
	m_Capabilities.MaxTextureArrayLayers = m_PhysicalDeviceLimits.maxImageArrayLayers;
	m_Capabilities.MaxInputBindings = m_PhysicalDeviceLimits.maxVertexInputBindings;
	m_Capabilities.MaxInputAttributes = m_PhysicalDeviceLimits.maxVertexInputAttributes;
	m_Capabilities.MaxComputeWorkGroupCountX = m_PhysicalDeviceLimits.maxComputeWorkGroupSize[0];
	m_Capabilities.MaxComputeWorkGroupCountY = m_PhysicalDeviceLimits.maxComputeWorkGroupSize[1];
	m_Capabilities.MaxComputeWorkGroupCountZ = m_PhysicalDeviceLimits.maxComputeWorkGroupSize[2];
	m_Capabilities.MaxViewports = m_PhysicalDeviceLimits.maxViewports;
	m_Capabilities.MaxFrameBufferWidth = m_PhysicalDeviceLimits.maxFramebufferWidth;
	m_Capabilities.MaxFrameBufferHeight = m_PhysicalDeviceLimits.maxFramebufferHeight;
	m_Capabilities.MaxFrameBufferLayers = m_PhysicalDeviceLimits.maxFramebufferLayers;
	m_Capabilities.MaxColorAttachments = m_PhysicalDeviceLimits.maxColorAttachments;
	m_Capabilities.MaxDrawIndexedIndexValue = m_PhysicalDeviceLimits.maxDrawIndexedIndexValue;
	m_Capabilities.MaxDrawIndirectNum = m_PhysicalDeviceLimits.maxDrawIndirectCount;
	m_Capabilities.MaxSamplersPerStage = m_PhysicalDeviceLimits.maxPerStageDescriptorSamplers;
	m_Capabilities.MaxResourcesPerStage = m_PhysicalDeviceLimits.maxPerStageResources;
	m_Capabilities.MaxDescriptorSets = m_PhysicalDeviceLimits.maxBoundDescriptorSets;
	m_Capabilities.MaxSamplerAnisotropy = m_PhysicalDeviceLimits.maxSamplerAnisotropy;
}

void VulkanDevice::WaitIdle() const
{
	assert(m_LogicalDevice);
	m_LogicalDevice.waitIdle();
}

VulkanDevice::~VulkanDevice()
{
	m_LogicalDevice.destroy();
	m_LogicalDevice = nullptr;
}
