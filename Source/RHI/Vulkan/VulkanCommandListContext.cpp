#include "RHI/Vulkan/VulkanQueue.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanCommandListContext.h"
#include "Engine/Services/SpdLogService.h"

VulkanCommandListContext::VulkanCommandListContext(const VulkanDevice& Device, VulkanQueue& Queue)
	: m_Pool(Device, Queue.GetFamilyIndex())
	, m_Queue(Queue)
{
}
