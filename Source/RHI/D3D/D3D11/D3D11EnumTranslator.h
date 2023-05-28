#pragma once

#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(Gfx)

class D3D11EnumTranslator
{
public:
	static D3D11_FILL_MODE polygonMode(EPolygonMode mode);
	static D3D11_CULL_MODE cullMode(ECullMode mode);
	static D3D11_BLEND_OP blendOp(EBlendOp op);
	static D3D11_BLEND blendFactor(EBlendFactor factor);
	static uint8_t renderTargetWriteMask(uint32_t colorMask);
	static D3D11_COMPARISON_FUNC compareFunc(ECompareFunc func);
	static D3D11_STENCIL_OP stencilOp(EStencilOp op);
	static DXGI_FORMAT format(EFormat format);
	static D3D11_FILTER filter(EFilter filter);
	static D3D11_TEXTURE_ADDRESS_MODE samplerAddressMode(ESamplerAddressMode addressMode);
	static D3D11_PRIMITIVE_TOPOLOGY primitiveTopology(EPrimitiveTopology primitiveTopology);
	static uint32_t bindFlags(uint32_t bindFlags);
};

NAMESPACE_END(Gfx)
