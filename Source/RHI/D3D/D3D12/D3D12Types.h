#pragma once

#include "Colorful/IRenderer/IImage.h"
#include "Colorful/IRenderer/IShader.h"
#include "Colorful/IRenderer/IBuffer.h"
#include "Colorful/D3D/DXGI_Interface.h"
#include <d3d12.h>

NAMESPACE_START(RHI)

namespace D3D12Type
{
	DXGI_FORMAT Format(EFormat Format);

	D3D12_RESOURCE_DIMENSION ImageDimension(EImageType Type);

	D3D12_SHADER_VISIBILITY ShaderVisibility(EShaderStage Stage);

	D3D12_LOGIC_OP LogicOp(ELogicOp Op);

	D3D12_BLEND_OP BlendOp(EBlendOp Op);

	D3D12_BLEND BlendFactor(EBlendFactor Factor);

	uint8_t ColorComponentMask(EColorWriteMask ColorMask);

	D3D12_FILL_MODE PolygonMode(EPolygonMode Mode);

	D3D12_CULL_MODE CullMode(ECullMode Mode);

	D3D12_COMPARISON_FUNC CompareFunc(ECompareFunc Func);

	D3D12_STENCIL_OP StencilOp(EStencilOp Op);

	D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopology(EPrimitiveTopology PrimitiveTopology);

	D3D12_FILTER Filter(EFilter MinmagFilter, EFilter MipmapFilter, uint32_t MaxAnisotropy);

	D3D12_TEXTURE_ADDRESS_MODE SamplerAddressMode(ESamplerAddressMode AddressMode);

	Vector4 BorderColor(EBorderColor Color);

	D3D12_RESOURCE_STATES ResourceStates(EResourceState State);
}

NAMESPACE_END(RHI)