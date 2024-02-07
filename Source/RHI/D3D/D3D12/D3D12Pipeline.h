#pragma once

#include "Engine/RHI/RHIPipeline.h"
#include "RHI/D3D/D3D12/D3D12Types.h"

class D3D12FrameBuffer : public RHIFrameBuffer
{
public:
	D3D12FrameBuffer(const RHIFrameBufferCreateInfo& RHICreateInfo)
	{
	}
};

class D3D12RootSignature : public D3DHwResource<ID3D12RootSignature>
{
public:
	D3D12RootSignature(const class D3D12Device& Device, const RHIGraphicsPipelineCreateInfo& RHICreateInfo);
protected:
	void Create(const class D3D12Device& Device, const std::vector<D3D12_ROOT_PARAMETER1>& Params, const RHIGraphicsPipelineCreateInfo& RHICreateInfo);
private:
};

class D3D12GraphicsPipelineState : public D3DHwResource<ID3D12PipelineState>
{
public:
	D3D12GraphicsPipelineState(const class D3D12Device& Device, const RHIGraphicsPipelineCreateInfo& RHICreateInfo);
protected:
#if 0
	void WriteImage(EShaderStage Stage, uint32_t Binding, IImage* Image) override final;
	void WriteSampler(EShaderStage Stage, uint32_t Binding, ISampler* Sampler) override final;
	void WriteUniformBuffer(EShaderStage Stage, uint32_t Binding, IBuffer* Buffer) override final;
#endif
private:
	std::unique_ptr<D3D12RootSignature> m_RootSignature;
};

class D3D12GraphicsPipeline : public RHIPipeline
{
public:
	D3D12GraphicsPipeline(const class D3D12Device& Device, const RHIGraphicsPipelineCreateInfo& RHICreateInfo);
protected:
private:
};
