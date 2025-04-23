#pragma once

#include "RHI/RHIDevice.h"
#include "RHI/D3D/D3D11/D3D11Types.h"

class D3D11Device final : public D3DHwResource<ID3D11Device>, public RHIDevice
{
public:
	D3D11Device(const DxgiFactory& Factory);

	void WaitIdle() const override final {}

	RHIShaderPtr CreateShader(const RHIShaderCreateInfo& RHICreateInfo) override final;
	RHITexturePtr CreateTexture(const RHITextureCreateInfo& RHICreateInfo) override final;
	RHIInputLayoutPtr CreateInputLayout(const RHIInputLayoutCreateInfo& RHICreateInfo) override final;
	RHIFrameBufferPtr CreateFrameBuffer(const RHIFrameBufferCreateInfo& RHICreateInfo) override final;
	RHIGraphicsPipelinePtr CreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& RHICreateInfo) override final;
	RHIPipelineStatePtr CreatePipelineState(const RHIGraphicsPipelineCreateInfo& RHICreateInfo) override final;
	RHIBufferPtr CreateBuffer(const RHIBufferCreateInfo& RHICreateInfo) override final;
	RHISamplerPtr CreateSampler(const RHISamplerCreateInfo& RHICreateInfo) override final;
	RHISwapchainPtr CreateSwapchain(const RHISwapchainCreateInfo& CreateInfo) override final;

	RHICommandListContext* GetImmediateCommandListContext(ERHIDeviceQueue Queue) override final;
	RHICommandListContextPtr AcquireDeferredCommandListContext() override final;
	void ReleaseDeferredCommandListContext(RHICommandListContextPtr& CmdListContext) override final;
#if 0
	D3D11CommandQueue* CommandQueue(EQueueType QueueType)
	{
		assert(QueueType <= EQueueType::Compute);

		static const uint32_t GraphicsQueueIndex = static_cast<uint32_t>(EQueueType::Graphics);
		const uint32_t QueueIndex = static_cast<uint32_t>(QueueType);

#if !USE_DEDICATE_TRANSFER_QUEUE
		if (QueueType == EQueueType::Transfer)
		{
			return m_Queues[GraphicsQueueIndex].get();
		}
#endif
		return m_Queues[QueueIndex] ? m_Queues[QueueIndex].get() : m_Queues[GraphicsQueueIndex].get();
	}
#endif
protected:
private:
	//std::array<std::unique_ptr<D3D11CommandQueue>, static_cast<size_t>(ERHIDeviceQueue::Compute) + 1ull> m_Queues;
	//std::array<std::unique_ptr<D3D11DescriptorAllocator>, D3D11_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> m_DescriptorAllocators;
	std::unique_ptr<DxgiAdapter> m_Adapter;
};
