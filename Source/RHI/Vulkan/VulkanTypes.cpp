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

vk::ImageType GetDimension(ERHITextureDimension Dimension)
{
	switch (Dimension)
	{
	case ERHITextureDimension::T_1D:
	case ERHITextureDimension::T_1D_Array:
		return vk::ImageType::e1D;
	case ERHITextureDimension::T_2D:
	case ERHITextureDimension::T_2D_Array:
	case ERHITextureDimension::T_Cube:
	case ERHITextureDimension::T_Cube_Array:
		return vk::ImageType::e2D;
	case ERHITextureDimension::T_3D:
		return vk::ImageType::e3D;
	default:
		return vk::ImageType::e2D;
	}
}

vk::ImageViewType GetImageViewDimension(ERHITextureDimension Dimension)
{
	switch (Dimension)
	{
	case ERHITextureDimension::T_1D:         return vk::ImageViewType::e1D;
	case ERHITextureDimension::T_1D_Array:   return vk::ImageViewType::e1DArray;
	case ERHITextureDimension::T_2D:         return vk::ImageViewType::e2D;
	case ERHITextureDimension::T_2D_Array:   return vk::ImageViewType::e2DArray;
	case ERHITextureDimension::T_Cube:       return vk::ImageViewType::eCube;
	case ERHITextureDimension::T_Cube_Array: return vk::ImageViewType::eCubeArray;
	case ERHITextureDimension::T_3D:         return vk::ImageViewType::e3D;
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
	case ERHICommandBufferLevel::Primary: return vk::CommandBufferLevel::ePrimary;
	case ERHICommandBufferLevel::Secondary: return vk::CommandBufferLevel::eSecondary;
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

vk::VertexInputRate GetInputRate(ERHIVertexInputRate InputRate)
{
	switch (InputRate)
	{
	case ERHIVertexInputRate::Vertex:
		return vk::VertexInputRate::eVertex;
	case ERHIVertexInputRate::Instance:
		return vk::VertexInputRate::eInstance;
	default:
		return vk::VertexInputRate::eVertex;
	}
}

vk::AttachmentLoadOp GetAttachmentLoadOp(ERHILoadOp LoadOp)
{
	switch (LoadOp)
	{
	default:
	case ERHILoadOp::None:
		return vk::AttachmentLoadOp::eNoneEXT;
	case ERHILoadOp::Load:
		return vk::AttachmentLoadOp::eLoad;
	case ERHILoadOp::Clear:
		return vk::AttachmentLoadOp::eClear;
	case ERHILoadOp::DontCare:
		return vk::AttachmentLoadOp::eDontCare;
	}
}

vk::AttachmentStoreOp GetAttachmentStoreOp(ERHIStoreOp StoreOp)
{
	switch (StoreOp)
	{
	default:
	case ERHIStoreOp::None:
		return vk::AttachmentStoreOp::eNone;
	case ERHIStoreOp::Store:
		return vk::AttachmentStoreOp::eStore;
	case ERHIStoreOp::DontCare:
		return vk::AttachmentStoreOp::eDontCare;
	}
}

vk::IndexType GetIndexType(ERHIIndexFormat Format)
{
	switch (Format)
	{
	default:
	case ERHIIndexFormat::UInt32:
		return vk::IndexType::eUint32;
	case ERHIIndexFormat::UInt16:
		return vk::IndexType::eUint16;
	}
}

vk::ImageLayout GetImageLayout(ERHIResourceState State)
{
	switch (State)
	{
	case ERHIResourceState::Unknown:
	case ERHIResourceState::Common:
	case ERHIResourceState::VertexBuffer:
	case ERHIResourceState::UniformBuffer:
	case ERHIResourceState::IndexBuffer:
	case ERHIResourceState::IndirectArgument:
	case ERHIResourceState::StreamOut:
	case ERHIResourceState::AccelerationStructure:
		return vk::ImageLayout::eUndefined;
	case ERHIResourceState::ShaderResource:
		return vk::ImageLayout::eShaderReadOnlyOptimal;
	case ERHIResourceState::UnorderedAccess:
		return vk::ImageLayout::eGeneral;
	case ERHIResourceState::RenderTarget:
		return vk::ImageLayout::eColorAttachmentOptimal;
	case ERHIResourceState::DepthWrite:
		return vk::ImageLayout::eDepthStencilAttachmentOptimal;
	case ERHIResourceState::DepthRead:
		return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
	case ERHIResourceState::TransferDst:
	case ERHIResourceState::ResolveDst:
		return vk::ImageLayout::eTransferDstOptimal;
	case ERHIResourceState::TransferSrc:
	case ERHIResourceState::ResolveSrc:
		return vk::ImageLayout::eTransferSrcOptimal;
	case ERHIResourceState::Present:
		return vk::ImageLayout::ePresentSrcKHR;
	case ERHIResourceState::ShadingRate:
		return vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR;
	default:
		return vk::ImageLayout::eUndefined;
	}
}
