#pragma once

#include "RHI/Vulkan/VulkanTexture.h"

class VulkanRenderPass final : public VkHwResource<vk::RenderPass>
{
public:
	VulkanRenderPass(const class VulkanDevice& Device, const RHIRenderPassDesc& Desc);

	VulkanRenderPass(const class VulkanDevice& Device, const RHIRenderPassDesc2& Desc);
	
	~VulkanRenderPass() = default;
protected:
private:
};

class VulkanFrameBuffer final : public VkHwResource<vk::Framebuffer>, public RHIFrameBuffer
{
public:
	VulkanFrameBuffer(const class VulkanDevice& Device, vk::RenderPass CompatibleRenderPass, const RHIFrameBufferDesc& Desc);
};