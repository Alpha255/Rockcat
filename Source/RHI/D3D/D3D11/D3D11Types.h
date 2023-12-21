#pragma once

#include "Runtime/Engine/RHI/RHIBuffer.h"
#include "Runtime/Engine/RHI/RHIShader.h"
#include "RHI/D3D/DXGIInterface.h"
#include <d3d11.h>

DXGI_FORMAT GetFormat(ERHIFormat Format);
D3D11_RESOURCE_DIMENSION GetImageDimension(ERHIImageType Type);
D3D11_LOGIC_OP GetLogicOp(ERHILogicOp Op);
D3D11_BLEND_OP GetBlendOp(ERHIBlendOp Op);
D3D11_BLEND GetBlendFactor(ERHIBlendFactor Factor);
uint8_t GetColorComponentMask(ERHIColorWriteMask ColorMask);
D3D11_FILL_MODE GetPolygonMode(ERHIPolygonMode Mode);
D3D11_CULL_MODE GetCullMode(ERHICullMode Mode);
D3D11_COMPARISON_FUNC GetCompareFunc(ERHICompareFunc Func);
D3D11_STENCIL_OP GetStencilOp(ERHIStencilOp Op);
D3D11_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopology(ERHIPrimitiveTopology PrimitiveTopology);
D3D11_FILTER GetFilter(ERHIFilter MinmagFilter, ERHIFilter MipmapFilter, uint32_t MaxAnisotropy);
D3D11_TEXTURE_ADDRESS_MODE GetSamplerAddressMode(ERHISamplerAddressMode AddressMode);
Math::Vector4 GetBorderColor(ERHIBorderColor Color);