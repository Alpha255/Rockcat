#pragma once

#include "Engine/RHI/RHIDevice.h"
#include "RHI/D3D/D3D12/D3D12Types.h"

class D3D12Device final : public D3DHwResource<ID3D12Device>, public RHIDevice
{
public:
	D3D12Device(const DxgiFactory& Factory);

	void WaitIdle() const override final {}

	RHIShaderPtr CreateShader(const RHIShaderCreateInfo& RHICreateInfo) override final;

	RHITexturePtr CreateTexture(const RHITextureCreateInfo& RHICreateInfo) override final;

	RHIInputLayoutPtr CreateInputLayout(const RHIInputLayoutCreateInfo& RHICreateInfo) override final;

	RHIFrameBufferPtr CreateFrameBuffer(const RHIFrameBufferCreateInfo& RHICreateInfo) override final;

	RHIGraphicsPipelinePtr CreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& RHICreateInfo) override final;

	RHIBufferPtr CreateBuffer(const RHIBufferCreateInfo& RHICreateInfo) override final;

	RHISamplerPtr CreateSampler(const RHISamplerCreateInfo& RHICreateInfo) override final;
#if 0
	D3D12CommandQueue* CommandQueue(EQueueType QueueType)
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
	//std::array<std::unique_ptr<D3D12CommandQueue>, static_cast<size_t>(ERHIDeviceQueue::Compute) + 1ull> m_Queues;
	//std::array<std::unique_ptr<D3D12DescriptorAllocator>, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> m_DescriptorAllocators;
	std::unique_ptr<DxgiAdapter> m_Adapter;
};
