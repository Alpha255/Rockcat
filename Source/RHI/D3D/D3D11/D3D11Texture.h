#pragma once

#include "RHI/RHITexture.h"
#include "RHI/D3D/D3D11/D3D11Types.h"

class D3D11Texture1D final : public D3DHwResource<ID3D11Texture1D>, public RHITexture
{
public:
	D3D11Texture1D(const class D3D11Device& Device, const RHITextureDesc& Desc);
};

class D3D11Texture2D final : public D3DHwResource<ID3D11Texture2D>, public RHITexture
{
public:
	D3D11Texture2D(const class D3D11Device& Device, const RHITextureDesc& Desc);
};

class D3D11Texture3D final : public D3DHwResource<ID3D11Texture3D>, public RHITexture
{
public:
	D3D11Texture3D(const class D3D11Device& Device, const RHITextureDesc& Desc);
};

class D3D11Sampler : public D3DHwResource<ID3D11SamplerState>, public RHISampler
{
public:
	D3D11Sampler(const class D3D11Device& Device, const RHISamplerDesc& Desc);
};