#pragma once

#include "Core/Singleton.h"
#include "RHI/Vulkan/VulkanTypes.h"

class VulkanUploadManager : public LazySingleton<VulkanUploadManager>, public VkBaseDeviceResource
{
public:
	void QueueUpload(const class VulkanBuffer* Buffer, const void* Data, size_t Size, size_t SrcOffset, size_t DstOffset);
	void QueueUpload(const class VulkanTexture* Texture, const void* Data, size_t Size, uint32_t ArrayLayer, uint32_t MipLevel, size_t SrcOffset);
	void QueueUpload(const class VulkanTexture* Texture, const void* Data, size_t Size, size_t SrcOffset);

	VulkanBuffer* AcquireStagingBuffer(size_t Size);
	void QueueReleaseStagingBuffer(VulkanBuffer* Buffer, uint64_t FenceCounter);
protected:
	ALLOW_ACCESS_LAZY(VulkanUploadManager)

	VulkanUploadManager(const class VulkanDevice& Device);
private:
	class VulkanCommandBuffer* GetUploadCommandBuffer();
	const class VulkanQueue& GetQueue();
};
