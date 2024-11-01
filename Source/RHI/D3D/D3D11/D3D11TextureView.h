#pragma once

#include "RHI/D3D/D3D11/D3D11Texture.h"

class D3D11ShaderResourceView : public D3DHwResource<ID3D11ShaderResourceView>
{
public:
	D3D11ShaderResourceView(const class D3D11Device& Device, const RHITextureCreateInfo& RHICreateInfo);
};

class D3D11RenderTargetView : public D3DHwResource<ID3D11RenderTargetView>
{
public:
	D3D11RenderTargetView(const class D3D11Device& Device, const RHITextureCreateInfo& RHICreateInfo);
};

class D3D11DepthStencilView : public D3DHwResource<ID3D11DepthStencilView>
{
public:
	D3D11DepthStencilView(const class D3D11Device& Device, const RHITextureCreateInfo& RHICreateInfo);
};

class D3D11UnorderedAccessView : public D3DHwResource<ID3D11UnorderedAccessView>
{
public:
	D3D11UnorderedAccessView(const class D3D11Device& Device);
};