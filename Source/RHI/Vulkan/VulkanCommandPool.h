#pragma once

#include "RHI/Vulkan/VulkanCommandBuffer.h"
#include "Runtime/Engine/RHI/RHIDevice.h"

class VulkanCommandPool final : public VkHwResource<vk::CommandPool>
{
public:
	VulkanCommandPool(const class VulkanDevice& Device, uint32_t QueueFamilyIndex, ERHIDeviceQueue Queue);

	~VulkanCommandPool();

	std::shared_ptr<VulkanCommandBuffer> GetOrAllocateCommandBuffer(ERHICommandBufferLevel Level);
protected:
private:
};
