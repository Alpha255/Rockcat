#pragma once

#include "RHI/Vulkan/VulkanTexture.h"

class VulkanRenderPass final : public VkHwResource<vk::RenderPass>
{
public:
	VulkanRenderPass(const class VulkanDevice& Device, const RHIRenderPassDesc& CreateInfo);
	
	~VulkanRenderPass() = default;
protected:
private:
};

class VulkanFramebuffer final : public VkHwResource<vk::Framebuffer>, public RHIFrameBuffer
{
public:
	VulkanFramebuffer(const class VulkanDevice& Device, vk::RenderPass CompatibleRenderPass, const RHIFrameBufferDesc& CreateInfo);
};