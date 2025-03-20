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
	GetGraphicsCommandBuffer();
}

void VulkanCommandListContext::SubmitGraphicsCommandBuffer()
{
	{
		std::lock_guard Locker(m_PrimaryCommandBufferList.Lock);
		if (m_PrimaryCommandBufferList.Graphics)
		{
			SubmitUploadCommandBuffer(m_PrimaryCommandBufferList.Upload);
		}
	}

	//{
	//	std::lock_guard Locker(m_SecondaryCommandBufferList.Lock);
	//	if (m_SecondaryCommandBufferList.Graphics)
	//	{
	//		assert(false);
	//	}
	//}
}

void VulkanCommandListContext::SubmitUploadCommandBuffer(RHICommandBuffer* UploadCommandBuffer)
{
	{
		std::lock_guard Locker(m_PrimaryCommandBufferList.Lock);
		auto CommandBuffer = UploadCommandBuffer ? UploadCommandBuffer : m_PrimaryCommandBufferList.Upload;
		if (CommandBuffer)
		{
		}
	}

	//{
	//	std::lock_guard Locker(m_SecondaryCommandBufferList.Lock);
	//	auto CommandBuffer = UploadCommandBuffer ? UploadCommandBuffer : m_SecondaryCommandBufferList.Upload;
	//	if (CommandBuffer)
	//	{
	//	}
	//}
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
