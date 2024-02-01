#pragma once

#include "Runtime/Engine/RHI/RHIDevice.h"
#include "RHI/Vulkan/VulkanLoader.h"

vk::PolygonMode GetPolygonMode(ERHIPolygonMode Mode);
vk::CullModeFlagBits GetCullMode(ERHICullMode Mode);
vk::FrontFace GetFrontFace(ERHIFrontFace FrontFace);
vk::LogicOp GetLogicOp(ERHILogicOp Op);
vk::BlendOp GetBlendOp(ERHIBlendOp Op);
vk::BlendFactor GetBlendFactor(ERHIBlendFactor Factor);
vk::ColorComponentFlags GetColorComponentFlags(ERHIColorWriteMask ColorMask);
vk::CompareOp GetCompareFunc(ERHICompareFunc Func);
vk::StencilOp GetStencilOp(ERHIStencilOp Op);
vk::Format GetFormat(ERHIFormat Format);
vk::ShaderStageFlagBits GetShaderStage(ERHIShaderStage Stage);
vk::ShaderStageFlags GetShaderStageFlags(ERHIShaderStage Stage);
vk::PrimitiveTopology GetPrimitiveTopology(ERHIPrimitiveTopology PrimitiveTopology);
vk::DescriptorType GetDescriptorType(ERHIResourceType Type);
vk::Filter GetFilter(ERHIFilter Filter);
vk::SamplerAddressMode GetSamplerAddressMode(ERHISamplerAddressMode AddressMode);
vk::BorderColor GetBorderColor(ERHIBorderColor BorderColor);
vk::ImageType GetImageType(ERHIImageType Type);
vk::SampleCountFlagBits GetSampleCount(ERHISampleCount SampleCount);
vk::ImageViewType GetImageViewType(ERHIImageType Type);
vk::DebugReportObjectTypeEXT GetDebugReportObjectType(vk::ObjectType Type);
vk::CommandBufferLevel GetCommandBufferLevel(ERHICommandBufferLevel Level);
#if 0
VkAttachmentLoadOp attachmentLoadOp(RenderPassDesc::EAttachmentLoadOp op);
VkAttachmentStoreOp attachmentStoreOp(RenderPassDesc::EAttachmentStoreOp op);
VkImageLayout imageLayout(Texture::EImageLayout layout);
VkPipelineStageFlags pipelineStageFlags(GfxFlags flags);
VkAccessFlags accessFlags(GfxFlags flags);
#endif
