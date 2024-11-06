#pragma once

#include "RHI/Vulkan/VulkanCommandPool.h"
#include "Engine/RHI/RHICommandListContext.h"

class VulkanCommandListContext : public RHICommandListContext
{
public:
	VulkanCommandListContext(const class VulkanDevice& Device, class VulkanQueue& Queue);

	RHICommandBufferPtr AllocateCommandBuffer(ERHICommandBufferLevel Level) override final
	{
		return m_Pool.AllocateCommandBuffer(Level);
	}

	void Submit() override final;
private:
	VulkanCommandPool m_Pool;
	class VulkanQueue& m_Queue;
};
