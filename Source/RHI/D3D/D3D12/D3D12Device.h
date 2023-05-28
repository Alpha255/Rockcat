#pragma once

#include "Colorful/D3D/D3D12/D3D12CommandQueue.h"
#include "Colorful/D3D/D3D12/D3D12Buffer.h"
#include "Colorful/D3D/D3D12/D3D12Shader.h"
#include "Colorful/D3D/D3D12/D3D12Image.h"
#include "Colorful/D3D/D3D12/D3D12Pipeline.h"
#include "Colorful/D3D/D3D12/D3D12Descriptor.h"

NAMESPACE_START(RHI)

struct D3D12Features
{
	bool8_t WaveOps = false;
	bool8_t ShadingRate = false;
	bool8_t MeshShader = false;
	D3D_ROOT_SIGNATURE_VERSION RootSignatureVersion = D3D_ROOT_SIGNATURE_VERSION_1;
	D3D12_RESOURCE_BINDING_TIER ResourceBindingTier = D3D12_RESOURCE_BINDING_TIER_1;
	D3D12_RESOURCE_HEAP_TIER ResourceHeapTier = D3D12_RESOURCE_HEAP_TIER_1;
	D3D12_VARIABLE_SHADING_RATE_TIER ShadingRateTier = D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED;
};

class D3D12Device final : public D3DHWObject<IDevice, ID3D12Device>
{
public:
	D3D12Device(DxgiFactory* Factory);

	void WaitIdle() override final
	{
	}

	IShaderSharedPtr CreateShader(const ShaderDesc& Desc) override final
	{
		return std::make_shared<D3D12Shader>(Desc);
	}

	IImageSharedPtr CreateImage(const ImageDesc& Desc) override final
	{
		return std::make_shared<D3D12Image>(this, Desc);
	}

	IInputLayoutSharedPtr CreateInputLayout(const InputLayoutDesc& Desc, const ShaderDesc&) override final
	{
		return std::make_shared<D3D12InputLayout>(Desc);
	}

	IFrameBufferSharedPtr CreateFrameBuffer(const FrameBufferDesc& Desc) override final
	{
		return std::make_shared<D3D12FrameBuffer>(Desc);
	}

	IPipelineSharedPtr CreateGraphicsPipeline(const GraphicsPipelineDesc& Desc) override final
	{
		return std::make_shared<D3D12GraphicsPipeline>(this, Desc);
	}

	IBufferSharedPtr CreateBuffer(const BufferDesc& Desc) override final
	{
		return std::make_shared<D3D12Buffer>(this, Desc);
	}

	ISamplerSharedPtr CreateSampler(const SamplerDesc& Desc) override final
	{
		return std::make_shared<D3D12Sampler>(this, Desc);
	}

	ICommandBufferPoolSharedPtr CreateCommandBufferPool(EQueueType QueueType) override final
	{
		(void)QueueType;
		return nullptr;
	}

	void SubmitCommandBuffers(EQueueType QueueType, const std::vector<ICommandBufferSharedPtr>& Commands) override final
	{
		(void)QueueType;
		(void)Commands;
	}

	void SubmitCommandBuffer(EQueueType QueueType, ICommandBufferSharedPtr& Command) override final
	{
		(void)QueueType;
		(void)Command;
	}

	void SubmitCommandBuffers(const std::vector<ICommandBufferSharedPtr>& Commands) override final
	{
		(void)Commands;
	}

	void SubmitCommandBuffer(ICommandBufferSharedPtr& Command) override final
	{
		(void)Command;
	}

	ICommandBufferSharedPtr GetOrAllocateCommandBuffer(EQueueType QueueType, ECommandBufferLevel Level, bool8_t AutoBegin, bool8_t UseForTransfer) override final
	{
		(void)QueueType;
		(void)Level;
		(void)AutoBegin;
		(void)UseForTransfer;
		return nullptr;
	}

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

	const D3D12Features& Features() const
	{
		return m_Features;
	}
protected:
private:
	D3D12Features m_Features;
	std::array<std::unique_ptr<D3D12CommandQueue>, static_cast<size_t>(EQueueType::Compute) + 1ull> m_Queues;
	std::array<std::unique_ptr<D3D12DescriptorAllocator>, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> m_DescriptorAllocators;
	std::unique_ptr<DxgiAdapter> m_Adapter;
};

NAMESPACE_END(RHI)
