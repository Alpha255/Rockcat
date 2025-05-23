#pragma once

#include "RHI/RHIDevice.h"
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
vk::ImageType GetDimension(ERHITextureDimension Dimension);
vk::SampleCountFlagBits GetSampleCount(ERHISampleCount SampleCount);
vk::ImageViewType GetImageViewDimension(ERHITextureDimension Dimension);
vk::DebugReportObjectTypeEXT GetDebugReportObjectType(vk::ObjectType Type);
vk::CommandBufferLevel GetCommandBufferLevel(ERHICommandBufferLevel Level);
vk::VertexInputRate GetInputRate(ERHIVertexInputRate InputRate);
vk::AttachmentLoadOp GetAttachmentLoadOp(ERHILoadOp LoadOp);
vk::AttachmentStoreOp GetAttachmentStoreOp(ERHIStoreOp StoreOp);
vk::IndexType GetIndexType(ERHIIndexFormat Format);
vk::ImageLayout GetImageLayout(ERHIResourceState Stage);
