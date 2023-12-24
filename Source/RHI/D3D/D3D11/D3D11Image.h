#pragma once

#include "Runtime/Engine/RHI/RHIImage.h"
#include "RHI/D3D/D3D11/D3D11Types.h"

class D3D11Image1D final : public D3DHwResource<ID3D11Texture1D>, public RHIImage
{
public:
	D3D11Image1D(const class D3D11Device& Device, const RHIImageCreateInfo& RHICreateInfo);
};

class D3D11Image2D final : public D3DHwResource<ID3D11Texture2D>, public RHIImage
{
public:
	D3D11Image2D(const class D3D11Device& Device, const RHIImageCreateInfo& RHICreateInfo);
};

class D3D11Image3D final : public D3DHwResource<ID3D11Texture3D>, public RHIImage
{
public:
	D3D11Image3D(const class D3D11Device& Device, const RHIImageCreateInfo& RHICreateInfo);
};

class D3D11Sampler : public D3DHwResource<ID3D11SamplerState>, public RHISampler
{
public:
	D3D11Sampler(const class D3D11Device& Device, const RHISamplerCreateInfo& RHICreateInfo);
};