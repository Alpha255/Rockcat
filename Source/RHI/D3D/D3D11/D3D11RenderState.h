#pragma once

#include "Colorful/D3D/D3D11/D3D11Device.h"

NAMESPACE_START(Gfx)

struct D3D11RenderState
{
	D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	uint32_t StencilRef = 0u;
	uint32_t BlendMask = 0u;
	Vec4 BlendFactor;

	ID3D11InputLayout* InputLayout = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;

	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11HullShader* HullShader = nullptr;
	ID3D11DomainShader* DomainShader = nullptr;
	ID3D11GeometryShader* GeometryShader = nullptr;
	ID3D11PixelShader* FragmentShader = nullptr;
	ID3D11ComputeShader* ComputeShader = nullptr;

	ID3D11RasterizerState* RasterizerState = nullptr;
	ID3D11BlendState* BlendState = nullptr;
	ID3D11DepthStencilState* DepthStencilState = nullptr;

	std::array<ID3D11RenderTargetView*, ELimitations::MaxRenderTargets> RenderTargets;
	ID3D11DepthStencilView* DepthStencilView = nullptr;
};

NAMESPACE_END(Gfx)