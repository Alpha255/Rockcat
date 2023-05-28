#pragma once

#include "Colorful/IRenderer/IImage.h"
#include "Colorful/D3D/D3D12/D3D12Types.h"

NAMESPACE_START(RHI)

class D3D12Image : public D3DHWObject<IImage, ID3D12Resource>
{
public:
	D3D12Image(class D3D12Device* Device, const ImageDesc& Desc);
protected:
private:
};

class D3D12Sampler : public ISampler
{
public:
	D3D12Sampler(class D3D12Device* Device, const SamplerDesc& Desc);

	~D3D12Sampler();
protected:
private:
	D3D12_CPU_DESCRIPTOR_HANDLE m_Descriptor{};
};

NAMESPACE_END(RHI)
