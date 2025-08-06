#pragma once

#include "RHI/D3D/D3D12/D3D12Types.h"
#include "RHI/RHITexture.h"

class D3D12Texture : public D3DHwResource<ID3D12Resource>, public RHITexture
{
public:
	D3D12Texture(const class D3D12Device& Device, const RHITextureDesc& Desc);
protected:
private:
};

class D3D12Sampler : public RHISampler
{
public:
	D3D12Sampler(const class D3D12Device& Device, const RHISamplerDesc& Desc);
protected:
private:
	D3D12_CPU_DESCRIPTOR_HANDLE m_Descriptor{};
};
