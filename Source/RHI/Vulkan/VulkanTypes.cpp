#include "Colorful/Vulkan/VulkanTypes.h"

NAMESPACE_START(RHI)

namespace VkType
{
	VkPolygonMode PolygonMode(EPolygonMode Mode)
	{
		switch (Mode)
		{
		case EPolygonMode::Solid:     return VK_POLYGON_MODE_FILL;
		case EPolygonMode::Wireframe: return VK_POLYGON_MODE_LINE;
		}
		return VK_POLYGON_MODE_MAX_ENUM;
	}

	VkCullModeFlags CullMode(ECullMode Mode)
	{
		switch (Mode)
		{
		case ECullMode::None:      return VK_CULL_MODE_NONE;
		case ECullMode::FrontFace: return VK_CULL_MODE_FRONT_BIT;
		case ECullMode::BackFace:  return VK_CULL_MODE_BACK_BIT;
		}
		return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
	}

	VkFrontFace FrontFace(EFrontFace FrontFace)
	{
		switch (FrontFace)
		{
		case EFrontFace::Counterclockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		case EFrontFace::Clockwise:        return VK_FRONT_FACE_CLOCKWISE;
		}
		return VK_FRONT_FACE_MAX_ENUM;
	}

	VkLogicOp LogicOp(ELogicOp Op)
	{
		switch (Op)
		{
		case ELogicOp::Clear:         return VK_LOGIC_OP_CLEAR;
		case ELogicOp::And:           return VK_LOGIC_OP_AND;
		case ELogicOp::And_Reverse:   return VK_LOGIC_OP_AND_REVERSE;
		case ELogicOp::Copy:          return VK_LOGIC_OP_COPY;
		case ELogicOp::And_Inverted:  return VK_LOGIC_OP_AND_INVERTED;
		case ELogicOp::No:            return VK_LOGIC_OP_NO_OP;
		case ELogicOp::Xor:           return VK_LOGIC_OP_XOR;
		case ELogicOp::Or:            return VK_LOGIC_OP_OR;
		case ELogicOp::Nor:           return VK_LOGIC_OP_NOR;
		case ELogicOp::Equivalent:    return VK_LOGIC_OP_EQUIVALENT;
		case ELogicOp::Invert:        return VK_LOGIC_OP_INVERT;
		case ELogicOp::Or_Reverse:    return VK_LOGIC_OP_OR_REVERSE;
		case ELogicOp::Copy_Inverted: return VK_LOGIC_OP_COPY_INVERTED;
		case ELogicOp::Or_Inverted:   return VK_LOGIC_OP_OR_INVERTED;
		case ELogicOp::Nand:          return VK_LOGIC_OP_NAND;
		case ELogicOp::Set:           return VK_LOGIC_OP_SET;
		}
		return VK_LOGIC_OP_MAX_ENUM;
	}

	VkBlendOp BlendOp(EBlendOp Op)
	{
		switch (Op)
		{
		case EBlendOp::Add:             return VK_BLEND_OP_ADD;
		case EBlendOp::Subtract:        return VK_BLEND_OP_SUBTRACT;
		case EBlendOp::ReverseSubtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
		case EBlendOp::Min:             return VK_BLEND_OP_MIN;
		case EBlendOp::Max:             return VK_BLEND_OP_MAX;
		}
		return VK_BLEND_OP_MAX_ENUM;
	}

	VkBlendFactor BlendFactor(EBlendFactor Factor)
	{
		switch (Factor)
		{
		case EBlendFactor::Zero:             return VK_BLEND_FACTOR_ZERO;
		case EBlendFactor::One:              return VK_BLEND_FACTOR_ONE;
		case EBlendFactor::Constant:         return VK_BLEND_FACTOR_CONSTANT_COLOR;
		case EBlendFactor::InverseConstant:  return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
		case EBlendFactor::SrcAlpha:         return VK_BLEND_FACTOR_SRC_ALPHA;
		case EBlendFactor::InverseSrcAlpha:  return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case EBlendFactor::DstAlpha:         return VK_BLEND_FACTOR_DST_ALPHA;
		case EBlendFactor::InverseDstAlpha:  return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		case EBlendFactor::SrcAlphaSaturate: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
		case EBlendFactor::Src1Alpha:        return VK_BLEND_FACTOR_SRC1_ALPHA;
		case EBlendFactor::InverseSrc1Alpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
		case EBlendFactor::SrcColor:         return VK_BLEND_FACTOR_SRC_COLOR;
		case EBlendFactor::InverseSrcColor:  return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case EBlendFactor::DstColor:         return VK_BLEND_FACTOR_DST_COLOR;
		case EBlendFactor::InverseDstColor:  return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case EBlendFactor::Src1Color:        return VK_BLEND_FACTOR_SRC1_COLOR;
		case EBlendFactor::InverseSrc1Color: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
		}
		return VK_BLEND_FACTOR_MAX_ENUM;
	}

	VkColorComponentFlags ColorComponentFlags(EColorWriteMask ColorMask)
	{
		VkColorComponentFlags Flags = 0u;
		if (ColorMask == EColorWriteMask::All)
		{
			return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		}
		if ((ColorMask & EColorWriteMask::Red) != EColorWriteMask::None)
		{
			Flags |= VK_COLOR_COMPONENT_R_BIT;
		}
		if ((ColorMask & EColorWriteMask::Green) != EColorWriteMask::None)
		{
			Flags |= VK_COLOR_COMPONENT_G_BIT;
		}
		if ((ColorMask & EColorWriteMask::Blue) != EColorWriteMask::None)
		{
			Flags |= VK_COLOR_COMPONENT_B_BIT;
		}
		if ((ColorMask & EColorWriteMask::Alpha) != EColorWriteMask::None)
		{
			Flags |= VK_COLOR_COMPONENT_A_BIT;
		}

		return Flags;
	}

	VkCompareOp CompareFunc(ECompareFunc Func)
	{
		switch (Func)
		{
		case ECompareFunc::Never:          return VK_COMPARE_OP_NEVER;
		case ECompareFunc::Less:           return VK_COMPARE_OP_LESS;
		case ECompareFunc::Equal:          return VK_COMPARE_OP_EQUAL;
		case ECompareFunc::LessOrEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL;
		case ECompareFunc::Greater:        return VK_COMPARE_OP_GREATER;
		case ECompareFunc::NotEqual:       return VK_COMPARE_OP_NOT_EQUAL;
		case ECompareFunc::GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case ECompareFunc::Always:         return VK_COMPARE_OP_ALWAYS;
		}
		return VK_COMPARE_OP_MAX_ENUM;
	}

	VkStencilOp StencilOp(EStencilOp Op)
	{
		switch (Op)
		{
		case EStencilOp::Keep:              return VK_STENCIL_OP_KEEP;
		case EStencilOp::Zero:              return VK_STENCIL_OP_ZERO;
		case EStencilOp::Replace:           return VK_STENCIL_OP_REPLACE;
		case EStencilOp::IncrementAndClamp: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
		case EStencilOp::DecrementAndClamp: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
		case EStencilOp::Invert:            return VK_STENCIL_OP_INVERT;
		case EStencilOp::IncrementAndWrap:  return VK_STENCIL_OP_INCREMENT_AND_WRAP;
		case EStencilOp::DecrementAndWrap:  return VK_STENCIL_OP_DECREMENT_AND_WRAP;
		}
		return VK_STENCIL_OP_MAX_ENUM;
	}

	VkFormat Format(EFormat Format)
	{
		return static_cast<VkFormat>(FormatAttribute::Attribute(Format).VkFormat);
	}

	VkShaderStageFlagBits ShaderStage(EShaderStage Stage)
	{
		switch (Stage)
		{
		case EShaderStage::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
		case EShaderStage::Hull:     return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case EShaderStage::Domain:   return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		case EShaderStage::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
		case EShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
		case EShaderStage::Compute:  return VK_SHADER_STAGE_COMPUTE_BIT;
		}
		return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	}

	VkPrimitiveTopology PrimitiveTopology(EPrimitiveTopology PrimitiveTopology)
	{
		switch (PrimitiveTopology)
		{
		case EPrimitiveTopology::PointList:              return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case EPrimitiveTopology::LineList:               return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case EPrimitiveTopology::LineStrip:              return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case EPrimitiveTopology::TriangleList:           return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case EPrimitiveTopology::TriangleStrip:          return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case EPrimitiveTopology::LineListAdjacency:      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
		case EPrimitiveTopology::LineStripAdjacency:     return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
		case EPrimitiveTopology::TriangleListAdjacency:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
		case EPrimitiveTopology::TriangleStripAdjacency: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
		case EPrimitiveTopology::PatchList:              return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		}
		return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
	}

	VkDescriptorType DescriptorType(EResourceType Type)
	{
		switch (Type)
		{
		case EResourceType::Sampler:              return VK_DESCRIPTOR_TYPE_SAMPLER;
		case EResourceType::CombinedImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case EResourceType::SampledImage:		  return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case EResourceType::StorageImage:         return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		case EResourceType::UniformTexelBuffer:   return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		case EResourceType::StorageTexelBuffer:   return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
		case EResourceType::UniformBuffer:        
		case EResourceType::PushConstants:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case EResourceType::StorageBuffer:        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case EResourceType::UniformBufferDynamic: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		case EResourceType::StorageBufferDynamic: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		case EResourceType::InputAttachment:      return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		}
		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}

	VkFilter Filter(EFilter Filter)
	{
		switch (Filter)
		{
		case EFilter::Nearest:  return VK_FILTER_NEAREST;
		case EFilter::Linear:   return VK_FILTER_LINEAR;
		}
		return VK_FILTER_MAX_ENUM;
	}

	VkSamplerAddressMode SamplerAddressMode(ESamplerAddressMode AddressMode)
	{
		switch (AddressMode)
		{
		case ESamplerAddressMode::Repeat:            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case ESamplerAddressMode::MirroredRepeat:    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case ESamplerAddressMode::ClampToEdge:       return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case ESamplerAddressMode::ClampToBorder:     return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case ESamplerAddressMode::MirrorClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		}
		return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
	}

	VkBorderColor BorderColor(EBorderColor BorderColor)
	{
		switch (BorderColor)
		{
		case EBorderColor::FloatTransparentBlack: return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		case EBorderColor::IntTransparentBlack:   return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
		case EBorderColor::FloatOpaqueBlack:      return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		case EBorderColor::IntOpaqueBlack:        return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		case EBorderColor::FloatOpaqueWhite:      return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		case EBorderColor::IntOpaqueWhite:        return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
		}
		return VK_BORDER_COLOR_MAX_ENUM;
	}

	VkImageType ImageType(EImageType Type)
	{
		switch (Type)
		{
		case EImageType::T_1D:
		case EImageType::T_1D_Array:
			return VK_IMAGE_TYPE_1D;
		case EImageType::T_2D:
		case EImageType::T_2D_Array:
		case EImageType::T_Cube:
		case EImageType::T_Cube_Array:
			return VK_IMAGE_TYPE_2D;
		case EImageType::T_3D:
			return VK_IMAGE_TYPE_3D;
		}

		assert(false);
		return VK_IMAGE_TYPE_MAX_ENUM;
	}

	VkImageViewType ImageViewType(EImageType Type)
	{
		switch (Type)
		{
		case EImageType::T_1D:         return VK_IMAGE_VIEW_TYPE_1D;
		case EImageType::T_1D_Array:   return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
		case EImageType::T_2D:         return VK_IMAGE_VIEW_TYPE_2D;
		case EImageType::T_2D_Array:   return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		case EImageType::T_Cube:       return VK_IMAGE_VIEW_TYPE_CUBE;
		case EImageType::T_Cube_Array: return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
		case EImageType::T_3D:         return VK_IMAGE_VIEW_TYPE_3D;
		}

		assert(false);
		return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	}

	VkDebugReportObjectTypeEXT DebugReportObjectType(EResourceType Type)
	{
		switch (Type)
		{
		case EResourceType::SampledImage:
		case EResourceType::StorageImage:
		case EResourceType::CombinedImageSampler: 
			return VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT;
		case EResourceType::InputAttachment:      
			return VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT;
		case EResourceType::UniformTexelBuffer:
		case EResourceType::StorageTexelBuffer:
		case EResourceType::StorageBuffer:
		case EResourceType::UniformBuffer:
		case EResourceType::StorageBufferDynamic:
		case EResourceType::UniformBufferDynamic: 
		case EResourceType::PushConstants:
			return VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT;
		case EResourceType::Sampler:              
			return VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT;
		}
		return VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT;
	}

	VkSampleCountFlagBits SampleCount(ESampleCount SampleCount)
	{
		switch (SampleCount)
		{
		case ESampleCount::Sample_1_Bit:  return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		case ESampleCount::Sample_2_Bit:  return VkSampleCountFlagBits::VK_SAMPLE_COUNT_2_BIT;
		case ESampleCount::Sample_4_Bit:  return VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT;
		case ESampleCount::Sample_8_Bit:  return VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT;
		case ESampleCount::Sample_16_Bit: return VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT;
		case ESampleCount::Sample_32_Bit: return VkSampleCountFlagBits::VK_SAMPLE_COUNT_32_BIT;
		case ESampleCount::Sample_64_Bit: return VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;
		}
		return VkSampleCountFlagBits::VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
	}

#if 0
	VkAttachmentLoadOp attachmentLoadOp(RenderPassDesc::EAttachmentLoadOp op)
	{
		switch (op)
		{
		case RenderPassDesc::EAttachmentLoadOp::Load:     return VK_ATTACHMENT_LOAD_OP_LOAD;
		case RenderPassDesc::EAttachmentLoadOp::Clear:    return VK_ATTACHMENT_LOAD_OP_CLEAR;
		case RenderPassDesc::EAttachmentLoadOp::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		}

		return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
	}

	VkAttachmentStoreOp attachmentStoreOp(RenderPassDesc::EAttachmentStoreOp op)
	{
		switch (op)
		{
		case RenderPassDesc::EAttachmentStoreOp::Store:    return VK_ATTACHMENT_STORE_OP_STORE;
		case RenderPassDesc::EAttachmentStoreOp::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}

		return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
	}

	VkImageLayout imageLayout(Texture::EImageLayout layout)
	{
		switch (layout)
		{
		case Texture::EImageLayout::Undefined:
			return VK_IMAGE_LAYOUT_UNDEFINED;
		case Texture::EImageLayout::TransferDst:
			return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		case Texture::EImageLayout::ColorAttachment:
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		case Texture::EImageLayout::DepthStencilAttachment:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		case Texture::EImageLayout::TransferSrc:
			return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		case Texture::EImageLayout::Present:
			return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		case Texture::EImageLayout::FragmentShaderRead:
		case Texture::EImageLayout::PixelDepthStencilRead:
		case Texture::EImageLayout::ComputeShaderReadWrite:
		case Texture::EImageLayout::FragmentShaderReadWrite:
			assert(0);
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}

		assert(0);
		return VK_IMAGE_LAYOUT_MAX_ENUM;
	}

	VkPipelineStageFlags pipelineStageFlags(GfxFlags flags)
	{
		static_assert(RenderPassDesc::EPipelineStageFlags::TopOfPipe == VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::DrawIndirect == VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::VertexInput == VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::VertexShader == VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::HullShader == VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::DomainShader == VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::GeometryShader == VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::FragmentShader == VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::BeforeEarlyZ == VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::AfterEarlyZ == VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::ColorAttachmentOutput == VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::ComputeShader == VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::Transfer == VK_PIPELINE_STAGE_TRANSFER_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::BottomOfPipe == VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::Host == VK_PIPELINE_STAGE_HOST_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::AllGraphics == VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::AllCommands == VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::TransformFeedback == VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::ConditionalRendering == VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::AcceleartionStructureBuild == VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::RayTracingShader == VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::ShadingRateImage == VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::TaskShader == VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::MeshShader == VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::FragmentDensityProcess == VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT, "Missmatched!!");
		static_assert(RenderPassDesc::EPipelineStageFlags::CommandPreProcess == VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV, "Missmatched!!");

		return static_cast<VkPipelineStageFlags>(flags);
	}

	VkAccessFlags accessFlags(GfxFlags flags)
	{
		static_assert(RenderPassDesc::EAccessFlags::IndirectCommandRead == VK_ACCESS_INDIRECT_COMMAND_READ_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::IndexRead == VK_ACCESS_INDEX_READ_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::VertexRead == VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::UniformRead == VK_ACCESS_UNIFORM_READ_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::InputAttachmentRead == VK_ACCESS_INPUT_ATTACHMENT_READ_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::ShaderRead == VK_ACCESS_SHADER_READ_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::ShaderWrite == VK_ACCESS_SHADER_WRITE_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::ColorAttachmentRead == VK_ACCESS_COLOR_ATTACHMENT_READ_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::ColorAttachmentWrite == VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::DepthStencilAttachmentRead == VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::DepthStencilAttachmentWrite == VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::TransferRead == VK_ACCESS_TRANSFER_READ_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::TransferWrite == VK_ACCESS_TRANSFER_WRITE_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::HostRead == VK_ACCESS_HOST_READ_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::HostWrite == VK_ACCESS_HOST_WRITE_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::MemoryRead == VK_ACCESS_MEMORY_READ_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::MemoryWrite == VK_ACCESS_MEMORY_WRITE_BIT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::TransformFeedbackWrite == VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::TransformFeedbackCounterRead == VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::TransformFeedbackCounterWrite == VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::ConditionalRenderingRead == VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::ColorAttachmentReadNonCoherent == VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::AcclerationStructureRead == VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::AcclerationStructureWrite == VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::ShadingRateImageRead == VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::FragmentDensityMapRead == VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::CommandPreProcessRead == VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV, "Missmatched");
		static_assert(RenderPassDesc::EAccessFlags::CommandPreProcessWrite == VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV, "Missmatched");

		return static_cast<VkAccessFlags>(flags);
	}
#endif
}

NAMESPACE_END(RHI)
