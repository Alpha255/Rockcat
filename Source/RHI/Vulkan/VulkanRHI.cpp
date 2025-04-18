#include "RHI/Vulkan/VulkanRHI.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanLayerExtensions.h"
#include "RHI/Vulkan/VulkanEnvConfiguration.h"

#if USE_DYNAMIC_VK_LOADER
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

std::shared_ptr<VulkanEnvConfiguration> VulkanRHI::s_EnvConfigs;

VulkanRHI::VulkanRHI()
{
	REGISTER_LOG_CATEGORY(LogVulkanRHI);
}

void VulkanRHI::InitializeGraphicsDevice()
{
#if USE_DYNAMIC_VK_LOADER
	const PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_DynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	assert(vkGetInstanceProcAddr);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);	
#endif
	s_EnvConfigs = VulkanEnvConfiguration::Load(VK_ENV_CONFIG_PATH);
	m_Device = std::make_unique<VulkanDevice>(s_EnvConfigs->ExtensionConfigs);
	s_EnvConfigs->Save(true);
}

VulkanRHI::~VulkanRHI()
{
	m_Device.reset();

	LOG_CAT_INFO(LogVulkanRHI, "Finalized");
}

RHIDevice& VulkanRHI::GetDevice()
{
	return *m_Device;
}

const VulkanExtensionConfiguration& VulkanRHI::GetExtConfigs()
{
	return s_EnvConfigs->ExtensionConfigs;
}

const VulkanDescriptorLimitationConfiguration& VulkanRHI::GetDescriptorLimitationConfigs()
{
	return s_EnvConfigs->DescriptorLimitationConfigs;
}

#if 0
VulkanRenderer::VulkanRenderer(const RenderSettings* Settings, uint64_t WindowHandle)
	: IRenderer(Settings)
{
	assert(WindowHandle);

#if USE_VK_LOADER
	VulkanLoader::Get().LoadGlobalFuncs();
#endif

	m_Instance = std::make_unique<VulkanInstance>(false);

#if USE_VK_LOADER
	VulkanLoader::Get().LoadInstanceFuncs(m_Instance->Get());
#endif

	m_Device = std::make_unique<VulkanDevice>(m_Instance.get());

	VulkanMemoryAllocator::Create(m_Device.get());

	m_Swapchain = std::make_unique<VulkanSwapchain>(
		m_Device.get(),
		WindowHandle,
		m_Settings->Resolution.Width,
		m_Settings->Resolution.Height,
		m_Settings->FullScreen,
		m_Settings->VSync,
		false);
}

void VulkanRenderer::AdvanceNextFrame()
{
	m_Swapchain->AcquireNextImage();
}

void VulkanRenderer::Present()
{
	/// #TODO: User dedicate present queue ???
	for (auto QueueType : std::array<EQueueType, 3u>{
		EQueueType::Graphics,
			EQueueType::Transfer,
			EQueueType::Compute})
	{
		m_Device->Queue(QueueType)->SubmitQueuedCommandBuffers();
	}

	m_Swapchain->Present();

	StagingBufferAllocator::Get().Submit();

	if (m_Device->Options().SyncType == ESyncType::ForceWaitQueueIdle)
	{
		///#TODO Or just wait device idle ???
		for (auto QueueType : std::array<EQueueType, 3u>{
			EQueueType::Graphics,
				EQueueType::Transfer,
				EQueueType::Compute})
		{
			m_Device->Queue(QueueType)->WaitIdle();
		}
	}
}

void VulkanRenderer::OnWindowResized(uint32_t Width, uint32_t Height)
{
	m_Swapchain->Resize(Width, Height);
}

IFrameBuffer* VulkanRenderer::SwapchainFrameBuffer()
{
	return m_Swapchain->CurrentFrameBuffer();
}

IDevice* VulkanRenderer::Device()
{
	return m_Device.get();
}

VulkanRenderer::~VulkanRenderer()
{
	m_Device->WaitIdle();

	AssetDatabase::Destroy();
	StagingBufferAllocator::Destroy();

	m_Swapchain.reset();

	VulkanMemoryAllocator::Destroy();

	LOG_DEBUG("Vulkan renderer finalized");
}

#if defined(DYNAMIC_LIBRARY)
extern "C"
{
	EXPORT_API void CreateRenderer(IRendererPtr& RendererPtr, const RenderSettings* Settings)
	{
		RendererPtr = std::make_unique<VulkanRenderer>(Settings);
	}
}
#endif
#endif
