#pragma once

#include "Colorful/Vulkan/VulkanBuffer.h"

/// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#memory-model

NAMESPACE_START(RHI)

class VulkanRenderPass final : public VkHWObject<void, VkRenderPass_T>
{
public:
	///VulkanRenderPass(class VulkanDevice* Device, const RenderPassDesc& desc);
	~VulkanRenderPass();
protected:
private:
};

class VulkanFramebuffer final : public VkHWObject<IFrameBuffer, VkFramebuffer_T>
{
public:
	VulkanFramebuffer(class VulkanDevice* Device, const FrameBufferDesc& Desc);

	~VulkanFramebuffer();

	VkRenderPass RenderPass() const
	{
		return m_RenderPass;
	}
protected:
	void CreateRenderPass(const FrameBufferDesc& Desc);
private:
	VkRenderPass m_RenderPass = VK_NULL_HANDLE;
};

NAMESPACE_END(RHI)