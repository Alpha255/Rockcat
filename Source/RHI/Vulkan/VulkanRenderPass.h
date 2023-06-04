#pragma once

#include "RHI/Vulkan/VulkanImage.h"

class VulkanRenderPass final : public VkHwResource<vk::RenderPass>
{
public:
	//VulkanRenderPass(const class VulkanDevice& Device, const RHIRenderPassCreateInfo& CreateInfo);
	
	~VulkanRenderPass() = default;
protected:
private:
};

class VulkanFramebuffer final : public VkHwResource<vk::Framebuffer>, public RHIFrameBuffer
{
public:
	VulkanFramebuffer(const class VulkanDevice& Device, const RHIFrameBufferCreateInfo& CreateInfo);

	~VulkanFramebuffer();
protected:
	void CreateRenderPass(const RHIFrameBufferCreateInfo& CreateInfo);
private:
	vk::RenderPass m_RenderPass;
};