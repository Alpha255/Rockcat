#pragma once

#include "Runtime/Engine/RHI/RHIBuffer.h"
#include "Runtime/Engine/RHI/RHIShader.h"
#include "RHI/D3D/DXGIInterface.h"
#include <d3d12.h>

DXGI_FORMAT GetFormat(ERHIFormat Format);
D3D12_RESOURCE_DIMENSION GetImageDimension(ERHIImageType Type);
D3D12_SHADER_VISIBILITY GetShaderVisibility(ERHIShaderStage Stage);
D3D12_LOGIC_OP GetLogicOp(ERHILogicOp Op);
D3D12_BLEND_OP GetBlendOp(ERHIBlendOp Op);
D3D12_BLEND GetBlendFactor(ERHIBlendFactor Factor);
uint8_t GetColorComponentMask(ERHIColorWriteMask ColorMask);
D3D12_FILL_MODE GetPolygonMode(ERHIPolygonMode Mode);
D3D12_CULL_MODE GetCullMode(ERHICullMode Mode);
D3D12_COMPARISON_FUNC GetCompareFunc(ERHICompareFunc Func);
D3D12_STENCIL_OP GetStencilOp(ERHIStencilOp Op);
D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopology(ERHIPrimitiveTopology PrimitiveTopology);
D3D12_FILTER GetFilter(ERHIFilter MinmagFilter, ERHIFilter MipmapFilter, uint32_t MaxAnisotropy);
D3D12_TEXTURE_ADDRESS_MODE GetSamplerAddressMode(ERHISamplerAddressMode AddressMode);
Math::Vector4 GetBorderColor(ERHIBorderColor Color);
D3D12_RESOURCE_STATES GetResourceStates(ERHIResourceState State);