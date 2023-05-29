#pragma once

#include "RHI/Vulkan/VulkanLoader.h"

namespace VkType
{
	VkPolygonMode PolygonMode(EPolygonMode Mode);
	VkCullModeFlags CullMode(ECullMode Mode);
	VkFrontFace FrontFace(EFrontFace FrontFace);
	VkLogicOp LogicOp(ELogicOp Op);
	VkBlendOp BlendOp(EBlendOp Op);
	VkBlendFactor BlendFactor(EBlendFactor Factor);
	VkColorComponentFlags ColorComponentFlags(EColorWriteMask ColorMask);
	VkCompareOp CompareFunc(ECompareFunc Func);
	VkStencilOp StencilOp(EStencilOp Op);
	VkFormat Format(EFormat Format);
	VkShaderStageFlagBits ShaderStage(EShaderStage Stage);
	VkPrimitiveTopology PrimitiveTopology(EPrimitiveTopology PrimitiveTopology);
	VkDescriptorType DescriptorType(EResourceType Type);
	VkFilter Filter(EFilter Filter);
	VkSamplerAddressMode SamplerAddressMode(ESamplerAddressMode AddressMode);
	VkBorderColor BorderColor(EBorderColor BorderColor);
	VkImageType ImageType(EImageType Type);
	VkSampleCountFlagBits SampleCount(ESampleCount SampleCount);
	VkImageViewType ImageViewType(EImageType Type);
	VkDebugReportObjectTypeEXT DebugReportObjectType(EResourceType Type);
#if 0
	VkAttachmentLoadOp attachmentLoadOp(RenderPassDesc::EAttachmentLoadOp op);
	VkAttachmentStoreOp attachmentStoreOp(RenderPassDesc::EAttachmentStoreOp op);
	VkImageLayout imageLayout(Texture::EImageLayout layout);
	VkPipelineStageFlags pipelineStageFlags(GfxFlags flags);
	VkAccessFlags accessFlags(GfxFlags flags);
#endif
};
