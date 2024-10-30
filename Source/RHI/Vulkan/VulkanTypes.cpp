#include "RHI/Vulkan/VulkanTypes.h"

vk::PolygonMode GetPolygonMode(ERHIPolygonMode Mode)
{
	switch (Mode)
	{
	case ERHIPolygonMode::Solid: return vk::PolygonMode::eFill;
	case ERHIPolygonMode::Wireframe: return vk::PolygonMode::eLine;    
	default:
		return vk::PolygonMode::eFill;
	}
}

vk::CullModeFlagBits GetCullMode(ERHICullMode Mode)
{
	switch (Mode)
	{
	case ERHICullMode::None: return vk::CullModeFlagBits::eNone;
	case ERHICullMode::FrontFace: return vk::CullModeFlagBits::eFront;
	case ERHICullMode::BackFace: return vk::CullModeFlagBits::eBack;
	case ERHICullMode::FrontAndBackFace: return vk::CullModeFlagBits::eFrontAndBack;
	default:
		return vk::CullModeFlagBits::eBack;
	}
}

vk::FrontFace GetFrontFace(ERHIFrontFace FrontFace)
{
	switch (FrontFace)
	{
	case ERHIFrontFace::Counterclockwise: return vk::FrontFace::eCounterClockwise;
	case ERHIFrontFace::Clockwise: return vk::FrontFace::eClockwise;
	default: 
		return vk::FrontFace::eCounterClockwise;
	}
}

vk::LogicOp GetLogicOp(ERHILogicOp Op)
{
	switch (Op)
	{
	case ERHILogicOp::Clear: return vk::LogicOp::eClear;
	case ERHILogicOp::And: return vk::LogicOp::eAnd;
	case ERHILogicOp::And_Reverse: return vk::LogicOp::eAndInverted;
	case ERHILogicOp::Copy: return vk::LogicOp::eCopy;
	case ERHILogicOp::And_Inverted: return vk::LogicOp::eAndInverted;
	case ERHILogicOp::No: return vk::LogicOp::eNoOp;
	case ERHILogicOp::Xor: return vk::LogicOp::eXor;
	case ERHILogicOp::Or: return vk::LogicOp::eOr;
	case ERHILogicOp::Nor: return vk::LogicOp::eNor;
	case ERHILogicOp::Equivalent: return vk::LogicOp::eEquivalent;
	case ERHILogicOp::Invert: return vk::LogicOp::eInvert;
	case ERHILogicOp::Or_Reverse: return vk::LogicOp::eOrReverse;
	case ERHILogicOp::Copy_Inverted: return vk::LogicOp::eCopyInverted;
	case ERHILogicOp::Or_Inverted: return vk::LogicOp::eOrInverted;
	case ERHILogicOp::Nand: return vk::LogicOp::eNand;
	case ERHILogicOp::Set: return vk::LogicOp::eSet;
	default:
		return vk::LogicOp::eAnd;
	}
}

vk::BlendOp GetBlendOp(ERHIBlendOp Op)
{
	switch (Op)
	{
	case ERHIBlendOp::Add:             return vk::BlendOp::eAdd;
	case ERHIBlendOp::Subtract:        return vk::BlendOp::eSubtract;
	case ERHIBlendOp::ReverseSubtract: return vk::BlendOp::eReverseSubtract;
	case ERHIBlendOp::Min:             return vk::BlendOp::eMin;
	case ERHIBlendOp::Max:             return vk::BlendOp::eMax;
	default:
		return vk::BlendOp::eAdd;
	}
}

vk::BlendFactor GetBlendFactor(ERHIBlendFactor Factor)
{
	switch (Factor)
	{
	case ERHIBlendFactor::Zero:             return vk::BlendFactor::eZero;
	case ERHIBlendFactor::One:              return vk::BlendFactor::eOne;
	case ERHIBlendFactor::Constant:         return vk::BlendFactor::eConstantColor;
	case ERHIBlendFactor::InverseConstant:  return vk::BlendFactor::eOneMinusConstantColor;
	case ERHIBlendFactor::SrcAlpha:         return vk::BlendFactor::eSrcAlpha;
	case ERHIBlendFactor::InverseSrcAlpha:  return vk::BlendFactor::eOneMinusSrcAlpha;
	case ERHIBlendFactor::DstAlpha:         return vk::BlendFactor::eDstAlpha;
	case ERHIBlendFactor::InverseDstAlpha:  return vk::BlendFactor::eOneMinusDstAlpha;
	case ERHIBlendFactor::SrcAlphaSaturate: return vk::BlendFactor::eSrcAlphaSaturate;
	case ERHIBlendFactor::Src1Alpha:        return vk::BlendFactor::eSrc1Alpha;
	case ERHIBlendFactor::InverseSrc1Alpha: return vk::BlendFactor::eOneMinusSrc1Alpha;
	case ERHIBlendFactor::SrcColor:         return vk::BlendFactor::eSrcColor;
	case ERHIBlendFactor::InverseSrcColor:  return vk::BlendFactor::eOneMinusSrcColor;
	case ERHIBlendFactor::DstColor:         return vk::BlendFactor::eDstColor;
	case ERHIBlendFactor::InverseDstColor:  return vk::BlendFactor::eOneMinusDstColor;
	case ERHIBlendFactor::Src1Color:        return vk::BlendFactor::eSrc1Color;
	case ERHIBlendFactor::InverseSrc1Color: return vk::BlendFactor::eOneMinusSrc1Color;
	default:
		return vk::BlendFactor::eOne;
	}
}

vk::ColorComponentFlags GetColorComponentFlags(ERHIColorWriteMask ColorMask)
{
	vk::ColorComponentFlags Flags;
	if (ColorMask == ERHIColorWriteMask::All)
	{
		return vk::ColorComponentFlags(vk::FlagTraits<vk::ColorComponentFlagBits>::allFlags);
	}
	if ((ColorMask & ERHIColorWriteMask::Red) != ERHIColorWriteMask::None)
	{
		Flags |= vk::ColorComponentFlagBits::eR;
	}
	if ((ColorMask & ERHIColorWriteMask::Green) != ERHIColorWriteMask::None)
	{
		Flags |= vk::ColorComponentFlagBits::eG;
	}
	if ((ColorMask & ERHIColorWriteMask::Blue) != ERHIColorWriteMask::None)
	{
		Flags |= vk::ColorComponentFlagBits::eB;
	}
	if ((ColorMask & ERHIColorWriteMask::Alpha) != ERHIColorWriteMask::None)
	{
		Flags |= vk::ColorComponentFlagBits::eA;
	}

	return Flags;
}

vk::CompareOp GetCompareFunc(ERHICompareFunc Func)
{
	switch (Func)
	{
	case ERHICompareFunc::Never:          return vk::CompareOp::eNever;
	case ERHICompareFunc::Less:           return vk::CompareOp::eLess;
	case ERHICompareFunc::Equal:          return vk::CompareOp::eEqual;
	case ERHICompareFunc::LessOrEqual:    return vk::CompareOp::eLessOrEqual;
	case ERHICompareFunc::Greater:        return vk::CompareOp::eGreater;
	case ERHICompareFunc::NotEqual:       return vk::CompareOp::eNotEqual;
	case ERHICompareFunc::GreaterOrEqual: return vk::CompareOp::eGreaterOrEqual;
	case ERHICompareFunc::Always:         return vk::CompareOp::eAlways;
	default:
		return vk::CompareOp::eAlways;
	}
}

vk::StencilOp GetStencilOp(ERHIStencilOp Op)
{
	switch (Op)
	{
	case ERHIStencilOp::Keep:              return vk::StencilOp::eKeep;
	case ERHIStencilOp::Zero:              return vk::StencilOp::eZero;
	case ERHIStencilOp::Replace:           return vk::StencilOp::eReplace;
	case ERHIStencilOp::IncrementAndClamp: return vk::StencilOp::eIncrementAndClamp;
	case ERHIStencilOp::DecrementAndClamp: return vk::StencilOp::eDecrementAndClamp;
	case ERHIStencilOp::Invert:            return vk::StencilOp::eInvert;
	case ERHIStencilOp::IncrementAndWrap:  return vk::StencilOp::eIncrementAndWrap;
	case ERHIStencilOp::DecrementAndWrap:  return vk::StencilOp::eDecrementAndWrap;
	default:
		return vk::StencilOp::eKeep;
	}
}

vk::Format GetFormat(ERHIFormat Format)
{
	return RHI::GetVulkanFormat(Format);
}

vk::ShaderStageFlagBits GetShaderStage(ERHIShaderStage Stage)
{
	switch (Stage)
	{
	case ERHIShaderStage::Vertex:   return vk::ShaderStageFlagBits::eVertex;
	case ERHIShaderStage::Hull:     return vk::ShaderStageFlagBits::eTessellationControl;
	case ERHIShaderStage::Domain:   return vk::ShaderStageFlagBits::eTessellationEvaluation;
	case ERHIShaderStage::Geometry: return vk::ShaderStageFlagBits::eGeometry;
	case ERHIShaderStage::Fragment: return vk::ShaderStageFlagBits::eFragment;
	case ERHIShaderStage::Compute:  return vk::ShaderStageFlagBits::eCompute;
	default:
		assert(0);
		return vk::ShaderStageFlagBits::eAll;
	}
}

vk::ShaderStageFlags GetShaderStageFlags(ERHIShaderStage Stage)
{
	vk::ShaderStageFlags ShaderStageFlags;
	if (EnumHasAnyFlags(Stage, ERHIShaderStage::Vertex))
	{
		ShaderStageFlags |= vk::ShaderStageFlagBits::eVertex;
	}
	if (EnumHasAnyFlags(Stage, ERHIShaderStage::Geometry))
	{
		ShaderStageFlags |= vk::ShaderStageFlagBits::eGeometry;
	}
	if (EnumHasAnyFlags(Stage, ERHIShaderStage::Fragment))
	{
		ShaderStageFlags |= vk::ShaderStageFlagBits::eFragment;
	}
	if (EnumHasAnyFlags(Stage, ERHIShaderStage::Compute))
	{
		ShaderStageFlags |= vk::ShaderStageFlagBits::eCompute;
	}
	return ShaderStageFlags;
}

vk::PrimitiveTopology GetPrimitiveTopology(ERHIPrimitiveTopology PrimitiveTopology)
{
	switch (PrimitiveTopology)
	{
	case ERHIPrimitiveTopology::PointList:              return vk::PrimitiveTopology::ePointList;
	case ERHIPrimitiveTopology::LineList:               return vk::PrimitiveTopology::eLineList;
	case ERHIPrimitiveTopology::LineStrip:              return vk::PrimitiveTopology::eLineStrip;
	case ERHIPrimitiveTopology::TriangleList:           return vk::PrimitiveTopology::eTriangleList;
	case ERHIPrimitiveTopology::TriangleStrip:          return vk::PrimitiveTopology::eTriangleStrip;
	case ERHIPrimitiveTopology::LineListAdjacency:      return vk::PrimitiveTopology::eLineListWithAdjacency;
	case ERHIPrimitiveTopology::LineStripAdjacency:     return vk::PrimitiveTopology::eLineStripWithAdjacency;
	case ERHIPrimitiveTopology::TriangleListAdjacency:  return vk::PrimitiveTopology::eTriangleListWithAdjacency;
	case ERHIPrimitiveTopology::TriangleStripAdjacency: return vk::PrimitiveTopology::eTriangleStripWithAdjacency;
	case ERHIPrimitiveTopology::PatchList:              return vk::PrimitiveTopology::ePatchList;
	default:
		return vk::PrimitiveTopology::eTriangleList;
	}
}

vk::DescriptorType GetDescriptorType(ERHIResourceType Type)
{
	switch (Type)
	{
	case ERHIResourceType::Sampler:              return vk::DescriptorType::eSampler;
	case ERHIResourceType::CombinedImageSampler: return vk::DescriptorType::eCombinedImageSampler;
	case ERHIResourceType::SampledImage:		 return vk::DescriptorType::eSampledImage;
	case ERHIResourceType::StorageImage:         return vk::DescriptorType::eStorageImage;
	case ERHIResourceType::UniformTexelBuffer:   return vk::DescriptorType::eUniformTexelBuffer;
	case ERHIResourceType::StorageTexelBuffer:   return vk::DescriptorType::eStorageTexelBuffer;
	case ERHIResourceType::UniformBuffer:
	case ERHIResourceType::PushConstants:        return vk::DescriptorType::eUniformBuffer;
	case ERHIResourceType::StorageBuffer:        return vk::DescriptorType::eStorageBuffer;
	case ERHIResourceType::UniformBufferDynamic: return vk::DescriptorType::eUniformBufferDynamic;
	case ERHIResourceType::StorageBufferDynamic: return vk::DescriptorType::eStorageBufferDynamic;
	case ERHIResourceType::InputAttachment:      return vk::DescriptorType::eInputAttachment;
	default:
		assert(0);
		return vk::DescriptorType::eSampledImage;
	}
}

vk::Filter GetFilter(ERHIFilter Filter)
{
	switch (Filter)
	{
	case ERHIFilter::Nearest:  return vk::Filter::eNearest;
	case ERHIFilter::Linear:   return vk::Filter::eLinear;
	default:
		return vk::Filter::eNearest;
	}
}

vk::SamplerAddressMode GetSamplerAddressMode(ERHISamplerAddressMode AddressMode)
{
	switch (AddressMode)
	{
	case ERHISamplerAddressMode::Repeat:            return vk::SamplerAddressMode::eRepeat;
	case ERHISamplerAddressMode::MirroredRepeat:    return vk::SamplerAddressMode::eMirroredRepeat;
	case ERHISamplerAddressMode::ClampToEdge:       return vk::SamplerAddressMode::eClampToEdge;
	case ERHISamplerAddressMode::ClampToBorder:     return vk::SamplerAddressMode::eClampToBorder;
	case ERHISamplerAddressMode::MirrorClampToEdge: return vk::SamplerAddressMode::eMirrorClampToEdge;
	default:
		return vk::SamplerAddressMode::eRepeat;
	}
}

vk::BorderColor GetBorderColor(ERHIBorderColor BorderColor)
{
	switch (BorderColor)
	{
	case ERHIBorderColor::FloatTransparentBlack: return vk::BorderColor::eFloatTransparentBlack;
	case ERHIBorderColor::IntTransparentBlack:   return vk::BorderColor::eIntTransparentBlack;
	case ERHIBorderColor::FloatOpaqueBlack:      return vk::BorderColor::eFloatOpaqueBlack;
	case ERHIBorderColor::IntOpaqueBlack:        return vk::BorderColor::eIntOpaqueBlack;
	case ERHIBorderColor::FloatOpaqueWhite:      return vk::BorderColor::eFloatOpaqueWhite;
	case ERHIBorderColor::IntOpaqueWhite:        return vk::BorderColor::eIntOpaqueWhite;
	default:
		return vk::BorderColor::eFloatTransparentBlack;
	}
}

vk::ImageType GetImageType(ERHIImageType Type)
{
	switch (Type)
	{
	case ERHIImageType::T_1D:
	case ERHIImageType::T_1D_Array:
		return vk::ImageType::e1D;
	case ERHIImageType::T_2D:
	case ERHIImageType::T_2D_Array:
	case ERHIImageType::T_Cube:
	case ERHIImageType::T_Cube_Array:
		return vk::ImageType::e2D;
	case ERHIImageType::T_3D:
		return vk::ImageType::e3D;
	default:
		return vk::ImageType::e2D;
	}
}

vk::ImageViewType GetImageViewType(ERHIImageType Type)
{
	switch (Type)
	{
	case ERHIImageType::T_1D:         return vk::ImageViewType::e1D;
	case ERHIImageType::T_1D_Array:   return vk::ImageViewType::e1DArray;
	case ERHIImageType::T_2D:         return vk::ImageViewType::e2D;
	case ERHIImageType::T_2D_Array:   return vk::ImageViewType::e2DArray;
	case ERHIImageType::T_Cube:       return vk::ImageViewType::eCube;
	case ERHIImageType::T_Cube_Array: return vk::ImageViewType::eCubeArray;
	case ERHIImageType::T_3D:         return vk::ImageViewType::e3D;
	default:
		return vk::ImageViewType::e2D;
	}
}

vk::DebugReportObjectTypeEXT GetDebugReportObjectType(vk::ObjectType Type)
{
	switch (Type)
	{
	case vk::ObjectType::eUnknown: return vk::DebugReportObjectTypeEXT::eUnknown;
	case vk::ObjectType::eInstance: return vk::DebugReportObjectTypeEXT::eInstance;
	case vk::ObjectType::ePhysicalDevice: return vk::DebugReportObjectTypeEXT::ePhysicalDevice;
	case vk::ObjectType::eDevice: return vk::DebugReportObjectTypeEXT::eDevice;
	case vk::ObjectType::eQueue: return vk::DebugReportObjectTypeEXT::eQueue;
	case vk::ObjectType::eSemaphore: return vk::DebugReportObjectTypeEXT::eSemaphore;
	case vk::ObjectType::eCommandBuffer: return vk::DebugReportObjectTypeEXT::eCommandBuffer;
	case vk::ObjectType::eFence: return vk::DebugReportObjectTypeEXT::eFence;
	case vk::ObjectType::eDeviceMemory: return vk::DebugReportObjectTypeEXT::eDeviceMemory;
	case vk::ObjectType::eBuffer: return vk::DebugReportObjectTypeEXT::eBuffer;
	case vk::ObjectType::eImage: return vk::DebugReportObjectTypeEXT::eImage;
	case vk::ObjectType::eEvent: return vk::DebugReportObjectTypeEXT::eEvent;
	case vk::ObjectType::eQueryPool: return vk::DebugReportObjectTypeEXT::eQueryPool;
	case vk::ObjectType::eBufferView: return vk::DebugReportObjectTypeEXT::eBufferView;
	case vk::ObjectType::eImageView: return vk::DebugReportObjectTypeEXT::eImageView;
	case vk::ObjectType::eShaderModule: return vk::DebugReportObjectTypeEXT::eShaderModule;
	case vk::ObjectType::ePipelineCache: return vk::DebugReportObjectTypeEXT::ePipelineCache;
	case vk::ObjectType::ePipelineLayout: return vk::DebugReportObjectTypeEXT::ePipelineLayout;
	case vk::ObjectType::eRenderPass: return vk::DebugReportObjectTypeEXT::eRenderPass;
	case vk::ObjectType::ePipeline: return vk::DebugReportObjectTypeEXT::ePipeline;
	case vk::ObjectType::eDescriptorSetLayout: return vk::DebugReportObjectTypeEXT::eDescriptorSetLayout;
	case vk::ObjectType::eSampler: return vk::DebugReportObjectTypeEXT::eSampler;
	case vk::ObjectType::eDescriptorPool: return vk::DebugReportObjectTypeEXT::eDescriptorPool;
	case vk::ObjectType::eDescriptorSet: return vk::DebugReportObjectTypeEXT::eDescriptorSet;
	case vk::ObjectType::eFramebuffer: return vk::DebugReportObjectTypeEXT::eFramebuffer;
	case vk::ObjectType::eCommandPool: return vk::DebugReportObjectTypeEXT::eCommandPool;
	case vk::ObjectType::eSamplerYcbcrConversion: return vk::DebugReportObjectTypeEXT::eSamplerYcbcrConversion;
	case vk::ObjectType::eDescriptorUpdateTemplate:return vk::DebugReportObjectTypeEXT::eDescriptorUpdateTemplate;
	case vk::ObjectType::eSurfaceKHR: return vk::DebugReportObjectTypeEXT::eSurfaceKHR;
	case vk::ObjectType::eSwapchainKHR: return vk::DebugReportObjectTypeEXT::eSwapchainKHR;
	case vk::ObjectType::eDisplayKHR: return vk::DebugReportObjectTypeEXT::eDisplayKHR;
	case vk::ObjectType::eDisplayModeKHR: return vk::DebugReportObjectTypeEXT::eDisplayModeKHR;
	case vk::ObjectType::eDebugReportCallbackEXT: return vk::DebugReportObjectTypeEXT::eDebugReportCallbackEXT;
	case vk::ObjectType::eCuModuleNVX: return vk::DebugReportObjectTypeEXT::eCuModuleNVX;
	case vk::ObjectType::eCuFunctionNVX: return vk::DebugReportObjectTypeEXT::eCuFunctionNVX;
	case vk::ObjectType::eAccelerationStructureKHR: return vk::DebugReportObjectTypeEXT::eAccelerationStructureKHR;
	case vk::ObjectType::eValidationCacheEXT: return vk::DebugReportObjectTypeEXT::eValidationCacheEXT;
	case vk::ObjectType::eAccelerationStructureNV: return vk::DebugReportObjectTypeEXT::eAccelerationStructureNV;
	default:
		return vk::DebugReportObjectTypeEXT::eUnknown;
	}
}

vk::CommandBufferLevel GetCommandBufferLevel(ERHICommandBufferLevel Level)
{
	switch (Level)
	{
	case ERHICommandBufferLevel::Immediate: return vk::CommandBufferLevel::ePrimary;
	case ERHICommandBufferLevel::Deferred: return vk::CommandBufferLevel::eSecondary;
	default: return vk::CommandBufferLevel::ePrimary;
	}
}

vk::SampleCountFlagBits GetSampleCount(ERHISampleCount SampleCount)
{
	switch (SampleCount)
	{
	case ERHISampleCount::Sample_1_Bit:  return vk::SampleCountFlagBits::e1;
	case ERHISampleCount::Sample_2_Bit:  return vk::SampleCountFlagBits::e2;
	case ERHISampleCount::Sample_4_Bit:  return vk::SampleCountFlagBits::e4;
	case ERHISampleCount::Sample_8_Bit:  return vk::SampleCountFlagBits::e8;
	case ERHISampleCount::Sample_16_Bit: return vk::SampleCountFlagBits::e16;
	case ERHISampleCount::Sample_32_Bit: return vk::SampleCountFlagBits::e32;
	case ERHISampleCount::Sample_64_Bit: return vk::SampleCountFlagBits::e64;
	default:
		return vk::SampleCountFlagBits::e1;
	}
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
