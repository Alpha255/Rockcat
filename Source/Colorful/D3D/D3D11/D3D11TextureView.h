#pragma once

#include "Colorful/D3D/D3D11/D3D11Texture.h"

NAMESPACE_START(Gfx)

class D3D11TextureView : public ITexture
{
protected:
	ID3D11Resource* createTexture(ID3D11Device* device, const TextureDesc& desc);
	std::unique_ptr<D3D11Texture> m_Texture = nullptr;
};

class D3D11ShaderResourceView : public D3DObject<ID3D11ShaderResourceView>, public D3D11TextureView
{
public:
	D3D11ShaderResourceView(ID3D11Device* device, const TextureDesc& desc);
};

class D3D11RenderTargetView : public D3DObject<ID3D11RenderTargetView>, public D3D11TextureView
{
public:
	D3D11RenderTargetView(ID3D11Device* device, const TextureDesc& desc);
};

class D3D11DepthStencilView : public D3DObject<ID3D11DepthStencilView>, public D3D11TextureView
{
public:
	D3D11DepthStencilView(ID3D11Device* device, const TextureDesc& desc);
};

class D3D11UnorderedAccessView : public D3DObject<ID3D11UnorderedAccessView>
{
public:
	D3D11UnorderedAccessView(ID3D11Device* device);
};

NAMESPACE_END(Gfx)