#include "RHI/Vulkan/VulkanUploadManager.h"
#include "RHI/Vulkan/VulkanRHI.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanQueue.h"
#include "RHI/Vulkan/VulkanCommandListContext.h"

VulkanUploadManager::VulkanUploadManager(const VulkanDevice& Device)
	: VkBaseDeviceResource(Device)
{
}

VulkanCommandBuffer* VulkanUploadManager::GetUploadCommandBuffer()
{
	RHICommandListContext* CommandListContext = nullptr;

	if (VulkanRHI::GetConfigs().UseTransferQueue)
	{
		CommandListContext = GetDevice().GetImmediateCommandListContext(ERHIDeviceQueue::Transfer);
	}
	else
	{
		thread_local std::shared_ptr<RHICommandListContext> t_CommandListContext;

		if (!t_CommandListContext)
		{
			t_CommandListContext = GetDevice().AcquireDeferredCommandListContext();
		}

		CommandListContext = t_CommandListContext.get();
	}

	assert(CommandListContext);
	return Cast<VulkanCommandBuffer>(CommandListContext->GetCommandBuffer());
}

const VulkanQueue& VulkanUploadManager::GetQueue()
{
	auto QueueIndex = VulkanRHI::GetConfigs().UseTransferQueue ? ERHIDeviceQueue::Transfer : ERHIDeviceQueue::Graphics;
	return GetDevice().GetQueue(QueueIndex);
}

void VulkanUploadManager::QueueUpload(const VulkanBuffer* Buffer, const void* Data, size_t Size, size_t SrcOffset, size_t DstOffset)
{
	assert(Buffer && Data && Size && SrcOffset < Size);

	auto CommandBuffer = GetUploadCommandBuffer();
	assert(CommandBuffer);

	CommandBuffer->WriteBuffer(Buffer, Data, Size, SrcOffset, DstOffset);

	GetQueue().Submit(CommandBuffer, 0u, nullptr);
}

void VulkanUploadManager::QueueUpload(const VulkanTexture* Texture, const void* Data, size_t Size, uint32_t ArrayLayer, uint32_t MipLevel, size_t SrcOffset)
{
	assert(Texture && Data && Size && SrcOffset < Size);

	auto CommandBuffer = GetUploadCommandBuffer();
	assert(CommandBuffer);

	CommandBuffer->WriteTexture(Texture, Data, Size, ArrayLayer, MipLevel, SrcOffset);

	GetQueue().Submit(CommandBuffer, 0u, nullptr);
}

void VulkanUploadManager::QueueUpload(const VulkanTexture* Texture, const void* Data, size_t Size, size_t SrcOffset)
{
	assert(Texture && Data && Size && SrcOffset < Size);

	auto CommandBuffer = GetUploadCommandBuffer();
	assert(CommandBuffer);

	CommandBuffer->WriteTexture(Texture, Data, Size, SrcOffset);

	GetQueue().Submit(CommandBuffer, 0u, nullptr);
}
