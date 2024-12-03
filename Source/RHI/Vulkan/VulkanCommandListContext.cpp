#include "RHI/Vulkan/VulkanQueue.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanCommandListContext.h"
#include "Engine/Services/SpdLogService.h"

VulkanCommandListContext::VulkanCommandListContext(const VulkanDevice& Device, VulkanQueue& Queue)
	: m_Pool(Device, Queue.GetFamilyIndex())
	, m_DescriptorPool(new VulkanDescriptorPool(Device))
	, m_Queue(Queue)
	, m_Device(Device)
{
	GetCommandBuffer();
}

void VulkanCommandListContext::Submit()
{
}

VulkanDescriptorPool& VulkanCommandListContext::AcquireDescriptorPool()
{
	if (m_DescriptorPool->IsFull())
	{
		m_FulledDescriptorPools.emplace_back(std::move(m_DescriptorPool));
		m_DescriptorPool = std::make_unique<VulkanDescriptorPool>(m_Device);
	}

	return *m_DescriptorPool;
}
