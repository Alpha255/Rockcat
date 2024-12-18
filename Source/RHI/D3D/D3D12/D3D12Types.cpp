#include "RHI/D3D/D3D12/D3D12Types.h"

DXGI_FORMAT Format(ERHIFormat Format)
{
	return RHI::GetDXGIFormat(Format);
}

D3D12_RESOURCE_DIMENSION GetDimension(ERHITextureDimension Dimension)
{
	switch (Dimension)
	{
	case ERHITextureDimension::T_1D:
	case ERHITextureDimension::T_1D_Array:
		return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
	case ERHITextureDimension::T_2D:
	case ERHITextureDimension::T_2D_Array:
	case ERHITextureDimension::T_Cube:
	case ERHITextureDimension::T_Cube_Array:
		return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	case ERHITextureDimension::T_3D:
		return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	default:
		return D3D12_RESOURCE_DIMENSION_UNKNOWN;
	}
}

D3D12_SHADER_VISIBILITY GetShaderVisibility(ERHIShaderStage Stage)
{
	switch (Stage)
	{
	case ERHIShaderStage::Vertex:
		return D3D12_SHADER_VISIBILITY_VERTEX;
	case ERHIShaderStage::Hull:
		return D3D12_SHADER_VISIBILITY_HULL;
	case ERHIShaderStage::Domain:
		return D3D12_SHADER_VISIBILITY_DOMAIN;
	case ERHIShaderStage::Geometry:
		return D3D12_SHADER_VISIBILITY_GEOMETRY;
	case ERHIShaderStage::Fragment:
		return D3D12_SHADER_VISIBILITY_PIXEL;
	case ERHIShaderStage::Compute:
		return D3D12_SHADER_VISIBILITY_ALL;
	default:
		return D3D12_SHADER_VISIBILITY_ALL;
	}
}

D3D12_LOGIC_OP GetLogicOp(ERHILogicOp Op)
{
	switch (Op)
	{
	case ERHILogicOp::Clear:         return D3D12_LOGIC_OP_CLEAR;
	case ERHILogicOp::And:           return D3D12_LOGIC_OP_AND;
	case ERHILogicOp::And_Reverse:   return D3D12_LOGIC_OP_AND_REVERSE;
	case ERHILogicOp::Copy:          return D3D12_LOGIC_OP_COPY;
	case ERHILogicOp::And_Inverted:  return D3D12_LOGIC_OP_AND_INVERTED;
	case ERHILogicOp::No:            return D3D12_LOGIC_OP_NOOP;
	case ERHILogicOp::Xor:           return D3D12_LOGIC_OP_XOR;
	case ERHILogicOp::Or:            return D3D12_LOGIC_OP_OR;
	case ERHILogicOp::Nor:           return D3D12_LOGIC_OP_NOR;
	case ERHILogicOp::Equivalent:    return D3D12_LOGIC_OP_EQUIV;
	case ERHILogicOp::Invert:        return D3D12_LOGIC_OP_INVERT;
	case ERHILogicOp::Or_Reverse:    return D3D12_LOGIC_OP_OR_REVERSE;
	case ERHILogicOp::Copy_Inverted: return D3D12_LOGIC_OP_COPY_INVERTED;
	case ERHILogicOp::Or_Inverted:   return D3D12_LOGIC_OP_OR_INVERTED;
	case ERHILogicOp::Nand:          return D3D12_LOGIC_OP_NAND;
	case ERHILogicOp::Set:           return D3D12_LOGIC_OP_SET;
	default:
		return D3D12_LOGIC_OP_AND;
	}
}

D3D12_BLEND_OP GetBlendOp(ERHIBlendOp Op)
{
	switch (Op)
	{
	case ERHIBlendOp::Add:             return D3D12_BLEND_OP_ADD;
	case ERHIBlendOp::Subtract:        return D3D12_BLEND_OP_SUBTRACT;
	case ERHIBlendOp::ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
	case ERHIBlendOp::Min:             return D3D12_BLEND_OP_MIN;
	case ERHIBlendOp::Max:             return D3D12_BLEND_OP_MAX;
	default:
		return D3D12_BLEND_OP_ADD;
	}
}

D3D12_BLEND GetBlendFactor(ERHIBlendFactor Factor)
{
	switch (Factor)
	{
	case ERHIBlendFactor::Zero:             return D3D12_BLEND_ZERO;
	case ERHIBlendFactor::One:              return D3D12_BLEND_ONE;
	case ERHIBlendFactor::Constant:         return D3D12_BLEND_BLEND_FACTOR;
	case ERHIBlendFactor::InverseConstant:  return D3D12_BLEND_INV_BLEND_FACTOR;
	case ERHIBlendFactor::SrcAlpha:         return D3D12_BLEND_SRC_ALPHA;
	case ERHIBlendFactor::InverseSrcAlpha:  return D3D12_BLEND_INV_SRC_ALPHA;
	case ERHIBlendFactor::DstAlpha:         return D3D12_BLEND_DEST_ALPHA;
	case ERHIBlendFactor::InverseDstAlpha:  return D3D12_BLEND_INV_DEST_ALPHA;
	case ERHIBlendFactor::SrcAlphaSaturate: return D3D12_BLEND_SRC_ALPHA_SAT;
	case ERHIBlendFactor::Src1Alpha:        return D3D12_BLEND_SRC1_ALPHA;
	case ERHIBlendFactor::InverseSrc1Alpha: return D3D12_BLEND_INV_SRC1_ALPHA;
	case ERHIBlendFactor::SrcColor:         return D3D12_BLEND_SRC_COLOR;
	case ERHIBlendFactor::InverseSrcColor:  return D3D12_BLEND_INV_SRC_COLOR;
	case ERHIBlendFactor::DstColor:         return D3D12_BLEND_DEST_COLOR;
	case ERHIBlendFactor::InverseDstColor:  return D3D12_BLEND_INV_DEST_COLOR;
	case ERHIBlendFactor::Src1Color:        return D3D12_BLEND_SRC1_COLOR;
	case ERHIBlendFactor::InverseSrc1Color: return D3D12_BLEND_INV_SRC1_COLOR;
	default:
		return D3D12_BLEND_ONE;
	}
}

uint8_t GetColorComponentMask(ERHIColorWriteMask ColorMask)
{
	uint8_t Mask = 0u;

	if (ColorMask == ERHIColorWriteMask::All)
	{
		return D3D12_COLOR_WRITE_ENABLE_ALL;
	}
	if (EnumHasAnyFlags(ColorMask, ERHIColorWriteMask::Red))
	{
		Mask |= D3D12_COLOR_WRITE_ENABLE_RED;
	}
	if (EnumHasAnyFlags(ColorMask, ERHIColorWriteMask::Green))
	{
		Mask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
	}
	if (EnumHasAnyFlags(ColorMask, ERHIColorWriteMask::Blue))
	{
		Mask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
	}
	if (EnumHasAnyFlags(ColorMask, ERHIColorWriteMask::Alpha))
	{
		Mask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
	}

	return Mask;
}

D3D12_FILL_MODE GetPolygonMode(ERHIPolygonMode Mode)
{
	switch (Mode)
	{
	case ERHIPolygonMode::Solid:     return D3D12_FILL_MODE_SOLID;
	case ERHIPolygonMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
	default:
		return D3D12_FILL_MODE_SOLID;
	}
}

D3D12_CULL_MODE GetCullMode(ERHICullMode Mode)
{
	switch (Mode)
	{
	case ERHICullMode::None:      return D3D12_CULL_MODE_NONE;
	case ERHICullMode::FrontFace: return D3D12_CULL_MODE_FRONT;
	case ERHICullMode::BackFace:  return D3D12_CULL_MODE_BACK;
	default:
		return D3D12_CULL_MODE_BACK;
	}
}

D3D12_COMPARISON_FUNC GetCompareFunc(ERHICompareFunc Func)
{
	switch (Func)
	{
	case ERHICompareFunc::Never:          return D3D12_COMPARISON_FUNC_NEVER;
	case ERHICompareFunc::Less:           return D3D12_COMPARISON_FUNC_LESS;
	case ERHICompareFunc::Equal:          return D3D12_COMPARISON_FUNC_EQUAL;
	case ERHICompareFunc::LessOrEqual:    return D3D12_COMPARISON_FUNC_LESS_EQUAL;
	case ERHICompareFunc::Greater:        return D3D12_COMPARISON_FUNC_GREATER;
	case ERHICompareFunc::NotEqual:       return D3D12_COMPARISON_FUNC_NOT_EQUAL;
	case ERHICompareFunc::GreaterOrEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	case ERHICompareFunc::Always:         return D3D12_COMPARISON_FUNC_ALWAYS;
	default:
		return D3D12_COMPARISON_FUNC_ALWAYS;
	}
}

D3D12_STENCIL_OP GetStencilOp(ERHIStencilOp Op)
{
	switch (Op)
	{
	case ERHIStencilOp::Keep:              return D3D12_STENCIL_OP_KEEP;
	case ERHIStencilOp::Zero:              return D3D12_STENCIL_OP_ZERO;
	case ERHIStencilOp::Replace:           return D3D12_STENCIL_OP_REPLACE;
	case ERHIStencilOp::IncrementAndClamp: return D3D12_STENCIL_OP_INCR_SAT;
	case ERHIStencilOp::DecrementAndClamp: return D3D12_STENCIL_OP_DECR_SAT;
	case ERHIStencilOp::Invert:            return D3D12_STENCIL_OP_INVERT;
	case ERHIStencilOp::IncrementAndWrap:  return D3D12_STENCIL_OP_INCR;
	case ERHIStencilOp::DecrementAndWrap:  return D3D12_STENCIL_OP_DECR;
	default:
		return D3D12_STENCIL_OP_KEEP;
	}
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(ERHIPrimitiveTopology Topology)
{
	switch (Topology)
	{
	case ERHIPrimitiveTopology::PointList:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	case ERHIPrimitiveTopology::LineList:
	case ERHIPrimitiveTopology::LineStrip:
	case ERHIPrimitiveTopology::LineListAdjacency:
	case ERHIPrimitiveTopology::LineStripAdjacency:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	case ERHIPrimitiveTopology::TriangleList:
	case ERHIPrimitiveTopology::TriangleStrip:
	case ERHIPrimitiveTopology::TriangleListAdjacency:
	case ERHIPrimitiveTopology::TriangleStripAdjacency:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	case ERHIPrimitiveTopology::PatchList:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	default:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}
}

D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology(ERHIPrimitiveTopology PrimitiveTopology)
{
	switch (PrimitiveTopology)
	{
	case ERHIPrimitiveTopology::PointList: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case ERHIPrimitiveTopology::LineList: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case ERHIPrimitiveTopology::LineStrip: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case ERHIPrimitiveTopology::LineListAdjacency: return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
	case ERHIPrimitiveTopology::LineStripAdjacency: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
	case ERHIPrimitiveTopology::TriangleList: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case ERHIPrimitiveTopology::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case ERHIPrimitiveTopology::TriangleListAdjacency: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
	case ERHIPrimitiveTopology::TriangleStripAdjacency: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
	case ERHIPrimitiveTopology::PatchList: return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
	default: return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}
}

D3D12_FILTER GetFilter(ERHIFilter MinmagFilter, ERHIFilter MipmapFilter, uint32_t MaxAnisotropy)
{
	if (MaxAnisotropy > 0u)
	{
		return D3D12_FILTER_ANISOTROPIC;
	}

	if (MinmagFilter == ERHIFilter::Nearest)
	{
		switch (MipmapFilter)
		{
		case ERHIFilter::Nearest: return D3D12_FILTER_MIN_MAG_MIP_POINT;
		case ERHIFilter::Linear: return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		}
	}
	else
	{
		switch (MipmapFilter)
		{
		case ERHIFilter::Nearest: return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case ERHIFilter::Linear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		}
	}

	return D3D12_FILTER_MIN_MAG_MIP_POINT;
}

D3D12_TEXTURE_ADDRESS_MODE GetSamplerAddressMode(ERHISamplerAddressMode AddressMode)
{
	switch (AddressMode)
	{
	case ERHISamplerAddressMode::Repeat:            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	case ERHISamplerAddressMode::MirroredRepeat:    return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	case ERHISamplerAddressMode::ClampToEdge:       return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	case ERHISamplerAddressMode::ClampToBorder:     return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	case ERHISamplerAddressMode::MirrorClampToEdge: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
	default:
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}
}

Math::Vector4 GetBorderColor(ERHIBorderColor Color)
{
	switch (Color)
	{
	case ERHIBorderColor::FloatTransparentBlack:
	case ERHIBorderColor::IntTransparentBlack:
		return Math::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	case ERHIBorderColor::FloatOpaqueBlack:
	case ERHIBorderColor::IntOpaqueBlack:
		return Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	case ERHIBorderColor::FloatOpaqueWhite:
	case ERHIBorderColor::IntOpaqueWhite:
		return Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	default:
		return Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}

D3D12_RESOURCE_STATES GetResourceStates(ERHIResourceState States)
{
	D3D12_RESOURCE_STATES Ret = D3D12_RESOURCE_STATE_COMMON;

	if (EnumHasAnyFlags(States, ERHIResourceState::VertexBuffer) || EnumHasAnyFlags(States, ERHIResourceState::UniformBuffer))
	{
		Ret |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::IndexBuffer))
	{
		Ret |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::RenderTarget))
	{
		Ret |= D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::UnorderedAccess))
	{
		Ret |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::DepthWrite))
	{
		Ret |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::DepthRead))
	{
		Ret |= D3D12_RESOURCE_STATE_DEPTH_READ;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::StreamOut))
	{
		Ret |= D3D12_RESOURCE_STATE_STREAM_OUT;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::IndirectArgument))
	{
		Ret |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::TransferDst))
	{
		Ret |= D3D12_RESOURCE_STATE_COPY_DEST;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::TransferSrc))
	{
		Ret |= D3D12_RESOURCE_STATE_COPY_SOURCE;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::ResolveDst))
	{
		Ret |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::ResolveSrc))
	{
		Ret |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::AccelerationStructure))
	{
		Ret |= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::ShadingRate))
	{
		Ret |= D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::ShaderResource))
	{
		Ret |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::Present))
	{
		Ret |= D3D12_RESOURCE_STATE_PRESENT;
	}

	if (EnumHasAnyFlags(States, ERHIResourceState::InputAttachment))
	{
		assert(false);
		Ret |= D3D12_RESOURCE_STATE_GENERIC_READ;
	}

	return Ret;
}
