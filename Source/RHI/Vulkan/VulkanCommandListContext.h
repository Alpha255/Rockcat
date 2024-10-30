#pragma once

#include "RHI/Vulkan/VulkanCommandPool.h"
#include "Engine/RHI/RHICommandListContext.h"

class VulkanCommandListContext : public RHICommandListContext
{
public:
	VulkanCommandListContext(const class VulkanDevice& Device, class VulkanQueue& Queue);


private:
	VulkanCommandPool m_Pool;
	class VulkanQueue& m_Queue;
};
