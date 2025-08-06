#pragma once

#include "RHI/RHIPipeline.h"
#include "RHI/D3D/D3D11/D3D11Types.h"

class D3D11RasterizerState : public D3DHwResource<ID3D11RasterizerState>
{
public:
	D3D11RasterizerState(const class D3D11Device& Device, const RHIRasterizationStateDesc& Desc, const RHIMultisampleStateDesc& MultisampleStateDesc);
};

class D3D11BlendState : public D3DHwResource<ID3D11BlendState>
{
public:
	D3D11BlendState(const class D3D11Device& Device, const RHIRasterizationStateDesc& Desc, const RHIMultisampleStateDesc& MultisampleStateDesc);
};

class D3D11DepthStencilState : public D3DHwResource<ID3D11DepthStencilState>
{
public:
	D3D11DepthStencilState(const class D3D11Device& Device, const RHIDepthStencilStateDesc& Desc);
};

class D3D11GraphicsPipeline
{
public:
	D3D11GraphicsPipeline(const class D3D11Device& Device, RHIGraphicsPipelineDesc& Desc);
protected:
private:
	std::unique_ptr<D3D11RasterizerState> m_RasterizerState = nullptr;
	std::unique_ptr<D3D11BlendState> m_BlendState = nullptr;
	std::unique_ptr<D3D11DepthStencilState> m_DepthStencilState = nullptr;
};

class D3D11ComputePipeline
{
};