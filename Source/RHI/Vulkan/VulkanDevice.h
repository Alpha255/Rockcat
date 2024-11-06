#pragma once

#include "RHI/Vulkan/VulkanTypes.h"
#include "Engine/RHI/RHIDevice.h"

class VulkanDevice final : public RHIDevice
{
public:
	VulkanDevice(struct VulkanLayerExtensionConfigurations* Configs);

	~VulkanDevice();

	void WaitIdle() const override final { assert(m_LogicalDevice); m_LogicalDevice.waitIdle(); }

	RHIShaderPtr CreateShader(const RHIShaderCreateInfo& CreateInfo) override final;
	RHITexturePtr CreateTexture(const RHITextureCreateInfo& CreateInfo) override final;
	RHIInputLayoutPtr CreateInputLayout(const RHIInputLayoutCreateInfo& CreateInfo) override final;
	RHIFrameBufferPtr CreateFrameBuffer(const RHIFrameBufferCreateInfo& CreateInfo) override final;
	RHIGraphicsPipelinePtr CreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& CreateInfo) override final;
	RHIBufferPtr CreateBuffer(const RHIBufferCreateInfo& CreateInfo) override final;
	RHISamplerPtr CreateSampler(const RHISamplerCreateInfo& CreateInfo) override final;

	RHICommandListContext* GetImmediateCommandListContext(ERHIDeviceQueue Queue) override final;
	RHICommandListContextPtr AcquireDeferredCommandListContext() override final;
	void ReleaseDeferredCommandListContext(RHICommandListContextPtr& CmdListContext) override final;

	inline const vk::Device& GetNative() const { return m_LogicalDevice; }
	inline const vk::PhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
	const vk::Instance& GetInstance() const;
	const vk::PhysicalDeviceLimits& GetPhysicalDeviceLimits() const { return m_Limits; }
private:
	std::unique_ptr<class VulkanInstance> m_Instance;
	std::array<std::unique_ptr<class VulkanQueue>, (size_t)ERHIDeviceQueue::Num> m_Queues;

	std::array<std::shared_ptr<class VulkanCommandListContext>, (size_t)ERHIDeviceQueue::Num> m_ImmediateCmdListContexts;
	std::queue<std::shared_ptr<class VulkanCommandListContext>> m_ThreadedCmdListContexts;

	vk::PhysicalDeviceLimits m_Limits;

	vk::PhysicalDevice m_PhysicalDevice;
	vk::Device m_LogicalDevice;
};
