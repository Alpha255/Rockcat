#pragma once

#include "RHI/Vulkan/VulkanTypes.h"

class VulkanQueue : public VkDeviceResource<vk::Queue>
{
public:
	VulkanQueue(const class VulkanDevice& Device, ERHIDeviceQueue QueueType, uint32_t QueueFamilyIndex);

	~VulkanQueue();

	const uint32_t GetFamilyIndex() const { return m_FamilyIndex; }
	const ERHIDeviceQueue GetType() const { return m_Type; }

	void Submit(class VulkanCommandBuffer* CommandBuffer, uint32_t NumSignalSemaphores, class VulkanSemaphore* SignalSemaphores) const;

	void WaitIdle() const;
protected:
private:
	ERHIDeviceQueue m_Type = ERHIDeviceQueue::Num;
	uint32_t m_FamilyIndex = ~0u;
};
