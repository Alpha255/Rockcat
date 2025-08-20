#pragma once

#include "RHI/Vulkan/VulkanTypes.h"
#include "RHI/Vulkan/VulkanDevelopSettings.h"
#include "RHI/RHIDevice.h"

class VulkanDevice final : public RHIDevice
{
public:
	VulkanDevice();
	~VulkanDevice();

	ERHIDeviceType GetType() const override final { return ERHIDeviceType::Vulkan; }

	void WaitIdle() const override final;

	RHIShaderPtr CreateShader(const RHIShaderDesc& Desc) override final;
	RHITexturePtr CreateTexture(const RHITextureDesc& Desc) override final;
	RHIInputLayoutPtr CreateInputLayout(const RHIInputLayoutDesc& Desc) override final;
	RHIFrameBufferPtr CreateFrameBuffer(const RHIFrameBufferDesc& Desc) override final;
	RHIGraphicsPipelinePtr CreateGraphicsPipeline(const RHIGraphicsPipelineDesc& Desc) override final;
	RHIPipelineStatePtr CreatePipelineState(const RHIGraphicsPipelineDesc& Desc) override final;
	RHIBufferPtr CreateBuffer(const RHIBufferDesc& Desc) override final;
	RHISamplerPtr CreateSampler(const RHISamplerDesc& Desc) override final;
	RHISwapchainPtr CreateSwapchain(const RHISwapchainDesc& Desc) override final;

	RHICommandListContext* GetImmediateCommandListContext(ERHIDeviceQueue Queue) override final;
	RHICommandListContextPtr AcquireDeferredCommandListContext() override final;
	void ReleaseDeferredCommandListContext(RHICommandListContextPtr& CmdListContext) override final;

	inline const vk::Device& GetNative() const { return m_LogicalDevice; }
	inline const vk::PhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }

	const vk::Instance& GetInstance() const;
	const vk::PhysicalDeviceLimits& GetPhysicalDeviceLimits() const { return m_PhysicalDeviceLimits; }

	const class VulkanQueue& GetQueue(ERHIDeviceQueue Queue) const;

	void SetupCapabilities() override final;

	inline const VulkanDevelopSettings& GetDevelopSettings() const { return *m_DevelopSettings; }
	inline const VulkanExtensionSettings& GetExtensionSettings() const { return m_DevelopSettings->ExtensionSettings; }
private:
	std::unique_ptr<class VulkanInstance> m_Instance;
	Array<std::unique_ptr<class VulkanQueue>, ERHIDeviceQueue> m_Queues;

	Array<std::shared_ptr<class VulkanCommandListContext>, ERHIDeviceQueue> m_ImmediateCmdListContexts;
	std::queue<std::shared_ptr<class VulkanCommandListContext>> m_ThreadedCmdListContexts;

	vk::PhysicalDeviceLimits m_PhysicalDeviceLimits;

	vk::PhysicalDevice m_PhysicalDevice;
	vk::Device m_LogicalDevice;

	std::shared_ptr<class VulkanPipelineCache> m_PipelineCache;
	std::shared_ptr<VulkanDevelopSettings> m_DevelopSettings;

#if USE_DYNAMIC_VK_LOADER
	const vk::detail::DynamicLoader m_DynamicLoader;
#endif
};
