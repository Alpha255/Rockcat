#pragma once

#include "Colorful/Vulkan/VulkanCommand.h"

NAMESPACE_START(RHI)

class VulkanCommandBufferPool final : public VkHWObject<ICommandBufferPool, VkCommandPool_T>
{
public:
	VulkanCommandBufferPool(class VulkanDevice* Device, uint32_t QueueFamilyIndex, EQueueType QueueType, VkCommandPoolCreateFlags CreateFlags);

	~VulkanCommandBufferPool();

	std::shared_ptr<VulkanCommandBuffer> GetOrAllocate(ECommandBufferLevel Level, bool8_t AutoBegin, bool8_t UseForTransfer);

	void Recycle(std::shared_ptr<VulkanCommandBuffer>& Command);

	void Free(std::shared_ptr<VulkanCommandBuffer>& Command);

	void Reset();

	EQueueType QueueType() const
	{
		return m_QueueType;
	}
protected:
private:
	std::list<std::shared_ptr<VulkanCommandBuffer>> m_PrimaryCommandBuffers;
	std::list<std::shared_ptr<VulkanCommandBuffer>> m_SecondaryCommandBuffers;
	EQueueType m_QueueType = EQueueType::Graphics;
};

NAMESPACE_END(RHI)
