#pragma once

#include "RHI/D3D/D3D12/D3D12Types.h"
#include "Runtime/Engine/RHI/RHIImage.h"

class D3D12Image : public D3DHwResource<ID3D12Resource>, public RHIImage
{
public:
	D3D12Image(const class D3D12Device& Device, const RHIImageCreateInfo& RHICreateInfo);
protected:
private:
};

class D3D12Sampler : public RHISampler
{
public:
	D3D12Sampler(const class D3D12Device& Device, const RHISamplerCreateInfo& RHICreateInfo);
protected:
private:
	D3D12_CPU_DESCRIPTOR_HANDLE m_Descriptor{};
};
