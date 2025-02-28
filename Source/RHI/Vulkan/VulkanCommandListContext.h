#pragma once

#include "RHI/Vulkan/VulkanCommandPool.h"
#include "RHI/Vulkan/VulkanDescriptor.h"
#include "Engine/RHI/RHICommandListContext.h"

class VulkanCommandListContext : public RHICommandListContext
{
public:
	VulkanCommandListContext(const class VulkanDevice& Device, class VulkanQueue& Queue);

	void SubmitActiveCommandBuffer() override final;

	VulkanDescriptorPool& AcquireDescriptorPool();
protected:
	RHICommandBufferPtr AllocateCommandBuffer(ERHICommandBufferLevel Level) override final
	{
		return m_Pool.AllocateCommandBuffer(Level);
	}
private:
	VulkanCommandPool m_Pool;
	std::vector<std::unique_ptr<VulkanDescriptorPool>> m_FulledDescriptorPools;
	std::unique_ptr<VulkanDescriptorPool> m_DescriptorPool;
	class VulkanQueue& m_Queue;
	const class VulkanDevice& m_Device;
};
