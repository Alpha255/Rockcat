#pragma once

#include "RHI/Vulkan/VulkanCommandBuffer.h"
#include "Engine/RHI/RHIDevice.h"

class VulkanCommandPool final : public VkHwResource<vk::CommandPool>
{
public:
	VulkanCommandPool(const class VulkanDevice& Device, uint32_t QueueFamilyIndex);

	~VulkanCommandPool();

	std::shared_ptr<VulkanCommandBuffer> AllocateCommandBuffer(ERHICommandBufferLevel Level);
	
	void Reset(bool ReleaseResources);

	void Free(std::shared_ptr<VulkanCommandBuffer>& CommandBuffer);
};

class VulkanCommandBufferManager
{
public:
	VulkanCommandBufferManager(const class VulkanDevice& Device, const class VulkanQueue& Queue);

	VulkanCommandBuffer* GetPrimaryCommandBuffer() const { return m_CommandBuffer.get(); }
private:
	VulkanCommandPool m_Pool;
	std::shared_ptr<VulkanCommandBuffer> m_CommandBuffer;
	std::queue<std::shared_ptr<VulkanCommandBuffer>> m_SecondaryCommandBuffers;
};
