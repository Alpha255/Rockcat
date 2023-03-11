#include "Colorful/D3D/D3D12/D3D12Types.h"

NAMESPACE_START(RHI)

namespace D3D12Type
{
	DXGI_FORMAT Format(EFormat Format)
	{
		return static_cast<DXGI_FORMAT>(FormatAttribute::Attribute(Format).DXGIFromat);
	}

	D3D12_RESOURCE_DIMENSION ImageDimension(EImageType Type)
	{
		switch (Type)
		{
		case EImageType::T_1D:
		case EImageType::T_1D_Array:
			return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		case EImageType::T_2D:
		case EImageType::T_2D_Array:
		case EImageType::T_Cube:
		case EImageType::T_Cube_Array:
			return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case EImageType::T_3D:
			return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		}

		return D3D12_RESOURCE_DIMENSION_UNKNOWN;
	}

	D3D12_SHADER_VISIBILITY ShaderVisibility(EShaderStage Stage)
	{
		switch (Stage)
		{
		case EShaderStage::Vertex:
			return D3D12_SHADER_VISIBILITY_VERTEX;
		case EShaderStage::Hull:
			return D3D12_SHADER_VISIBILITY_HULL;
		case EShaderStage::Domain:
			return D3D12_SHADER_VISIBILITY_DOMAIN;
		case EShaderStage::Geometry:
			return D3D12_SHADER_VISIBILITY_GEOMETRY;
		case EShaderStage::Fragment:
			return D3D12_SHADER_VISIBILITY_PIXEL;
		case EShaderStage::Compute:
			return D3D12_SHADER_VISIBILITY_ALL;
		}

		assert(false);
		return D3D12_SHADER_VISIBILITY_ALL;
	}

	D3D12_LOGIC_OP LogicOp(ELogicOp Op)
	{
		switch (Op)
		{
		case ELogicOp::Clear:         return D3D12_LOGIC_OP_CLEAR;
		case ELogicOp::And:           return D3D12_LOGIC_OP_AND;
		case ELogicOp::And_Reverse:   return D3D12_LOGIC_OP_AND_REVERSE;
		case ELogicOp::Copy:          return D3D12_LOGIC_OP_COPY;
		case ELogicOp::And_Inverted:  return D3D12_LOGIC_OP_AND_INVERTED;
		case ELogicOp::No:            return D3D12_LOGIC_OP_NOOP;
		case ELogicOp::Xor:           return D3D12_LOGIC_OP_XOR;
		case ELogicOp::Or:            return D3D12_LOGIC_OP_OR;
		case ELogicOp::Nor:           return D3D12_LOGIC_OP_NOR;
		case ELogicOp::Equivalent:    return D3D12_LOGIC_OP_EQUIV;
		case ELogicOp::Invert:        return D3D12_LOGIC_OP_INVERT;
		case ELogicOp::Or_Reverse:    return D3D12_LOGIC_OP_OR_REVERSE;
		case ELogicOp::Copy_Inverted: return D3D12_LOGIC_OP_COPY_INVERTED;
		case ELogicOp::Or_Inverted:   return D3D12_LOGIC_OP_OR_INVERTED;
		case ELogicOp::Nand:          return D3D12_LOGIC_OP_NAND;
		case ELogicOp::Set:           return D3D12_LOGIC_OP_SET;
		}
		return D3D12_LOGIC_OP_CLEAR;
	}

	D3D12_BLEND_OP BlendOp(EBlendOp Op)
	{
		switch (Op)
		{
		case EBlendOp::Add:             return D3D12_BLEND_OP_ADD;
		case EBlendOp::Subtract:        return D3D12_BLEND_OP_SUBTRACT;
		case EBlendOp::ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
		case EBlendOp::Min:             return D3D12_BLEND_OP_MIN;
		case EBlendOp::Max:             return D3D12_BLEND_OP_MAX;
		}
		return D3D12_BLEND_OP_ADD;
	}

	D3D12_BLEND BlendFactor(EBlendFactor Factor)
	{
		switch (Factor)
		{
		case EBlendFactor::Zero:             return D3D12_BLEND_ZERO;
		case EBlendFactor::One:              return D3D12_BLEND_ONE;
		case EBlendFactor::Constant:         return D3D12_BLEND_BLEND_FACTOR;
		case EBlendFactor::InverseConstant:  return D3D12_BLEND_INV_BLEND_FACTOR;
		case EBlendFactor::SrcAlpha:         return D3D12_BLEND_SRC_ALPHA;
		case EBlendFactor::InverseSrcAlpha:  return D3D12_BLEND_INV_SRC_ALPHA;
		case EBlendFactor::DstAlpha:         return D3D12_BLEND_DEST_ALPHA;
		case EBlendFactor::InverseDstAlpha:  return D3D12_BLEND_INV_DEST_ALPHA;
		case EBlendFactor::SrcAlphaSaturate: return D3D12_BLEND_SRC_ALPHA_SAT;
		case EBlendFactor::Src1Alpha:        return D3D12_BLEND_SRC1_ALPHA;
		case EBlendFactor::InverseSrc1Alpha: return D3D12_BLEND_INV_SRC1_ALPHA;
		case EBlendFactor::SrcColor:         return D3D12_BLEND_SRC_COLOR;
		case EBlendFactor::InverseSrcColor:  return D3D12_BLEND_INV_SRC_COLOR;
		case EBlendFactor::DstColor:         return D3D12_BLEND_DEST_COLOR;
		case EBlendFactor::InverseDstColor:  return D3D12_BLEND_INV_DEST_COLOR;
		case EBlendFactor::Src1Color:        return D3D12_BLEND_SRC1_COLOR;
		case EBlendFactor::InverseSrc1Color: return D3D12_BLEND_INV_SRC1_COLOR;
		}
		return D3D12_BLEND_ZERO;
	}

	uint8_t ColorComponentMask(EColorWriteMask ColorMask)
	{
		uint8_t Mask = 0u;

		if (ColorMask == EColorWriteMask::All)
		{
			return D3D12_COLOR_WRITE_ENABLE_ALL;
		}
		if (EnumHasAnyFlags(ColorMask, EColorWriteMask::Red))
		{
			Mask |= D3D12_COLOR_WRITE_ENABLE_RED;
		}
		if (EnumHasAnyFlags(ColorMask, EColorWriteMask::Green))
		{
			Mask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
		}
		if (EnumHasAnyFlags(ColorMask, EColorWriteMask::Blue))
		{
			Mask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
		}
		if (EnumHasAnyFlags(ColorMask, EColorWriteMask::Alpha))
		{
			Mask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
		}

		return Mask;
	}

	D3D12_FILL_MODE PolygonMode(EPolygonMode Mode)
	{
		switch (Mode)
		{
		case EPolygonMode::Solid:     return D3D12_FILL_MODE_SOLID;
		case EPolygonMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
		}
		return D3D12_FILL_MODE_SOLID;
	}

	D3D12_CULL_MODE CullMode(ECullMode Mode)
	{
		switch (Mode)
		{
		case ECullMode::None:      return D3D12_CULL_MODE_NONE;
		case ECullMode::FrontFace: return D3D12_CULL_MODE_FRONT;
		case ECullMode::BackFace:  return D3D12_CULL_MODE_BACK;
		}
		return D3D12_CULL_MODE_NONE;
	}

	D3D12_COMPARISON_FUNC CompareFunc(ECompareFunc Func)
	{
		switch (Func)
		{
		case ECompareFunc::Never:          return D3D12_COMPARISON_FUNC_NEVER;
		case ECompareFunc::Less:           return D3D12_COMPARISON_FUNC_LESS;
		case ECompareFunc::Equal:          return D3D12_COMPARISON_FUNC_EQUAL;
		case ECompareFunc::LessOrEqual:    return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case ECompareFunc::Greater:        return D3D12_COMPARISON_FUNC_GREATER;
		case ECompareFunc::NotEqual:       return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		case ECompareFunc::GreaterOrEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case ECompareFunc::Always:         return D3D12_COMPARISON_FUNC_ALWAYS;
		}
		return D3D12_COMPARISON_FUNC_NEVER;
	}

	D3D12_STENCIL_OP StencilOp(EStencilOp Op)
	{
		switch (Op)
		{
		case EStencilOp::Keep:              return D3D12_STENCIL_OP_KEEP;
		case EStencilOp::Zero:              return D3D12_STENCIL_OP_ZERO;
		case EStencilOp::Replace:           return D3D12_STENCIL_OP_REPLACE;
		case EStencilOp::IncrementAndClamp: return D3D12_STENCIL_OP_INCR_SAT;
		case EStencilOp::DecrementAndClamp: return D3D12_STENCIL_OP_DECR_SAT;
		case EStencilOp::Invert:            return D3D12_STENCIL_OP_INVERT;
		case EStencilOp::IncrementAndWrap:  return D3D12_STENCIL_OP_INCR;
		case EStencilOp::DecrementAndWrap:  return D3D12_STENCIL_OP_DECR;
		}
		return D3D12_STENCIL_OP_KEEP;
	}

	D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopology(EPrimitiveTopology Topology)
	{
		switch (Topology)
		{
		case EPrimitiveTopology::PointList:              return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		case EPrimitiveTopology::LineList:               return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case EPrimitiveTopology::LineStrip:              return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case EPrimitiveTopology::TriangleList:           return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case EPrimitiveTopology::TriangleStrip:          return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case EPrimitiveTopology::LineListAdjacency:      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case EPrimitiveTopology::LineStripAdjacency:     return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case EPrimitiveTopology::TriangleListAdjacency:  return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case EPrimitiveTopology::TriangleStripAdjacency: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case EPrimitiveTopology::PatchList:              return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		}
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}

	D3D12_FILTER Filter(EFilter MinmagFilter, EFilter MipmapFilter, uint32_t MaxAnisotropy)
	{
		if (MaxAnisotropy > 0u)
		{
			return D3D12_FILTER_ANISOTROPIC;
		}

		if (MinmagFilter == EFilter::Nearest)
		{
			switch (MipmapFilter)
			{
			case EFilter::Nearest: return D3D12_FILTER_MIN_MAG_MIP_POINT;
			case EFilter::Linear: return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
			}
		}
		else
		{
			switch (MipmapFilter)
			{
			case EFilter::Nearest: return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			case EFilter::Linear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			}
		}

		return D3D12_FILTER_MIN_MAG_MIP_POINT;
	}

	D3D12_TEXTURE_ADDRESS_MODE SamplerAddressMode(ESamplerAddressMode AddressMode)
	{
		switch (AddressMode)
		{
		case ESamplerAddressMode::Repeat:            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case ESamplerAddressMode::MirroredRepeat:    return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case ESamplerAddressMode::ClampToEdge:       return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case ESamplerAddressMode::ClampToBorder:     return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		case ESamplerAddressMode::MirrorClampToEdge: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		}
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}

	Vector4 BorderColor(EBorderColor Color)
	{
		switch (Color)
		{
			case EBorderColor::FloatTransparentBlack:
			case EBorderColor::IntTransparentBlack:
				return Vector4(0.0f, 0.0f, 0.0f, 0.0f);
			case EBorderColor::FloatOpaqueBlack:
			case EBorderColor::IntOpaqueBlack:
				return Vector4(0.0f, 0.0f, 0.0f, 1.0f);
			case EBorderColor::FloatOpaqueWhite:
			case EBorderColor::IntOpaqueWhite:
				return Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		return Color::Black;
	}

	D3D12_RESOURCE_STATES ResourceStates(EResourceState States)
	{
		D3D12_RESOURCE_STATES Ret = D3D12_RESOURCE_STATE_COMMON;

		if (EnumHasAnyFlags(States, EResourceState::VertexBuffer) || EnumHasAnyFlags(States, EResourceState::UniformBuffer))
		{
			Ret |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		}

		if (EnumHasAnyFlags(States, EResourceState::IndexBuffer))
		{
			Ret |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
		}

		if (EnumHasAnyFlags(States, EResourceState::RenderTarget))
		{
			Ret |= D3D12_RESOURCE_STATE_RENDER_TARGET;
		}

		if (EnumHasAnyFlags(States, EResourceState::UnorderedAccess))
		{
			Ret |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		}

		if (EnumHasAnyFlags(States, EResourceState::DepthWrite))
		{
			Ret |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}

		if (EnumHasAnyFlags(States, EResourceState::DepthRead))
		{
			Ret |= D3D12_RESOURCE_STATE_DEPTH_READ;
		}

		if (EnumHasAnyFlags(States, EResourceState::StreamOut))
		{
			Ret |= D3D12_RESOURCE_STATE_STREAM_OUT;
		}

		if (EnumHasAnyFlags(States, EResourceState::IndirectArgument))
		{
			Ret |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		}

		if (EnumHasAnyFlags(States, EResourceState::TransferDst))
		{
			Ret |= D3D12_RESOURCE_STATE_COPY_DEST;
		}

		if (EnumHasAnyFlags(States, EResourceState::TransferSrc))
		{
			Ret |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		}

		if (EnumHasAnyFlags(States, EResourceState::ResolveDst))
		{
			Ret |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
		}

		if (EnumHasAnyFlags(States, EResourceState::ResolveSrc))
		{
			Ret |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		}

		if (EnumHasAnyFlags(States, EResourceState::AccelerationStructure))
		{
			Ret |= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
		}

		if (EnumHasAnyFlags(States, EResourceState::ShadingRate))
		{
			Ret |= D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
		}

		if (EnumHasAnyFlags(States, EResourceState::ShaderResource))
		{
			Ret |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		}

		if (EnumHasAnyFlags(States, EResourceState::Present))
		{
			Ret |= D3D12_RESOURCE_STATE_PRESENT;
		}

		if (EnumHasAnyFlags(States, EResourceState::InputAttachment))
		{
			assert(false);
			Ret |= D3D12_RESOURCE_STATE_GENERIC_READ;
		}

		return Ret;
	}
}

NAMESPACE_END(RHI)
