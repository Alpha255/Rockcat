#pragma once

#include "Colorful/IRenderer/IPipeline.h"
#include "Colorful/D3D/D3D12/D3D12Types.h"

NAMESPACE_START(RHI)

class D3D12FrameBuffer : public IFrameBuffer
{
public:
	D3D12FrameBuffer(const FrameBufferDesc& Desc)
		: IFrameBuffer(Desc.Width, Desc.Height, Desc.Depth, Desc)
	{
	}
};

class D3D12RootSignature : public D3DHWObject<void, ID3D12RootSignature>
{
public:
	D3D12RootSignature(class D3D12Device* Device, const GraphicsPipelineDesc& Desc);
protected:
	void Create(class D3D12Device* Device, const std::vector<D3D12_ROOT_PARAMETER1>& Params, const GraphicsPipelineDesc& Desc);
private:
};

class D3D12GraphicsPipelineState : public D3DHWObject<PipelineState, ID3D12PipelineState>
{
public:
	D3D12GraphicsPipelineState(class D3D12Device* Device, const GraphicsPipelineDesc& Desc);
protected:
	void WriteImage(EShaderStage Stage, uint32_t Binding, IImage* Image) override final;
	void WriteSampler(EShaderStage Stage, uint32_t Binding, ISampler* Sampler) override final;
	void WriteUniformBuffer(EShaderStage Stage, uint32_t Binding, IBuffer* Buffer) override final;
private:
	std::unique_ptr<D3D12RootSignature> m_RootSignature;
};

class D3D12GraphicsPipeline : public IPipeline
{
public:
	D3D12GraphicsPipeline(class D3D12Device* Device, const GraphicsPipelineDesc& Desc);
protected:
private:
};

NAMESPACE_END(RHI)
