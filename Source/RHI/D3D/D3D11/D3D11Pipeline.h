#pragma once

#include "Colorful/D3D/D3D11/D3D11Texture.h"
#include "Colorful/D3D/D3D11/D3D11Shader.h"

NAMESPACE_START(Gfx)

class D3D11RasterizerState : public D3DObject<ID3D11RasterizerState>
{
public:
	D3D11RasterizerState(ID3D11Device* device, const RasterizationStateDesc& rasterizationStateDesc, const MultisampleStateDesc& multisampleStateDesc);
};

class D3D11BlendState : public D3DObject<ID3D11BlendState>
{
public:
	D3D11BlendState(ID3D11Device* device, const BlendStateDesc& blendStateDesc, const MultisampleStateDesc& multisampleStateDesc);
};

class D3D11DepthStencilState : public D3DObject<ID3D11DepthStencilState>
{
public:
	D3D11DepthStencilState(ID3D11Device* device, const DepthStencilStateDesc& desc);
};

class D3D11GraphicsPipeline
{
public:
	D3D11GraphicsPipeline(ID3D11Device* device, GraphicsPipelineDesc& desc);

	inline ID3D11RasterizerState* rasterizerState() const
	{
		assert(m_RasterizerState);
		return m_RasterizerState->get();
	}

	inline ID3D11BlendState* blendState() const
	{
		assert(m_BlendState);
		return m_BlendState->get();
	}

	inline ID3D11DepthStencilState* depthStencilState() const
	{
		assert(m_DepthStencilState);
		return m_DepthStencilState->get();
	}

	inline ID3D11InputLayout* inputLayout() const
	{
		assert(m_InputLayout);
		return m_InputLayout->get();
	}
protected:
private:
	std::unique_ptr<D3D11RasterizerState> m_RasterizerState = nullptr;
	std::unique_ptr<D3D11BlendState> m_BlendState = nullptr;
	std::unique_ptr<D3D11DepthStencilState> m_DepthStencilState = nullptr;
};

class D3D11ComputePipeline
{
};

/// null
class D3D11RayTracingPipeline
{
};

NAMESPACE_END(Gfx)