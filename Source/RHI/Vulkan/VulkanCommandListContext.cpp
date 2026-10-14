#include "RHI/Vulkan/VulkanQueue.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanCommandBuffer.h"
#include "RHI/Vulkan/VulkanCommandListContext.h"

VulkanCommandListContext::VulkanCommandListContext(const VulkanDevice& Device, VulkanQueue& Queue)
	: m_Pool(Device, Queue.GetFamilyIndex())
	, m_DescriptorPool(new VulkanDescriptorPool(Device))
	, m_Queue(Queue)
	, m_Device(Device)
{
	GetGraphicsCommandBuffer();
}

void VulkanCommandListContext::SubmitCommandBuffer(RHICommandBuffer* CommandBuffer)
{
	if (!CommandBuffer)
	{
		return;
	}

	auto VulkanCommand = static_cast<VulkanCommandBuffer*>(CommandBuffer);
	assert(VulkanCommand->IsEnded());

	m_Queue.Submit(VulkanCommand, 0u, nullptr);
}

void VulkanCommandListContext::SubmitUploadCommandBuffer(RHICommandBuffer* UploadCommandBuffer)
{
	{
		std::lock_guard Locker(m_PrimaryCommandBufferList.Lock);

		auto CommandBuffer = UploadCommandBuffer ? UploadCommandBuffer : m_PrimaryCommandBufferList.Upload;
		if (CommandBuffer)
		{
			/// RHIUploadManager records its staging -> device copies here. Without this the upload
			/// command buffer is never ended nor submitted, so anything uploaded through it
			/// (textures, buffer initial data) would silently never reach the GPU.
			EndAndMarkSubmitted(CommandBuffer);

			m_Queue.Submit(static_cast<VulkanCommandBuffer*>(CommandBuffer), 0u, nullptr);

			/// Drop the cached handle so the next upload goes through GetCommandBuffer() again and
			/// gets recycled. Recording into a submitted command buffer is invalid.
			m_PrimaryCommandBufferList.Upload = nullptr;
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
