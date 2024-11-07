#pragma once

#include "RHI/Vulkan/VulkanCommandPool.h"
#include "Engine/RHI/RHICommandListContext.h"

class VulkanCommandListContext : public RHICommandListContext
{
public:
	VulkanCommandListContext(const class VulkanDevice& Device, class VulkanQueue& Queue);

	void Submit() override final;

protected:
	RHICommandBufferPtr AllocateCommandBuffer(ERHICommandBufferLevel Level) override final
	{
		return m_Pool.AllocateCommandBuffer(Level);
	}
private:
	VulkanCommandPool m_Pool;
	class VulkanQueue& m_Queue;
};
