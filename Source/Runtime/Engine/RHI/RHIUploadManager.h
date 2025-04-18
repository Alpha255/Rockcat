#pragma once

#include "Core/Singleton.h"
#include "Engine/RHI/RHIBuffer.h"

class RHIUploadManager : public LazySingleton<RHIUploadManager>
{
public:
	inline size_t GetAllocatedMemorySize() const { return m_AllocatedMemorySize.load(); }
	inline size_t GetUsedMemorySize() const { return m_UsedMemorySize.load(); }

	void QueueUploadBuffer(const RHIBuffer* Buffer, const void* Data, size_t Size, size_t SrcOffset = 0u);
	void QueueUploadTexture(const RHITexture* Texture, const void* Data, size_t Size, size_t SrcOffset = 0u);
	void QueueUploadTexture(const RHITexture* Texture, uint32_t ArrayLayer, uint32_t MipLevel, const void* Data, size_t Size, size_t SrcOffset = 0u);

	void FlushPendingFreeStagingBuffers();
protected:
	ALLOW_ACCESS_LAZY(RHIUploadManager);

	RHIUploadManager(RHIDevice& Device)
		: m_Device(Device)
	{
	}
private:
	struct StagingBufferBlock;

	struct StagingBuffer
	{
		RHIBuffer* Buffer = nullptr;
		RHICommandBuffer* CommandBuffer = nullptr;
		StagingBufferBlock* Owner = nullptr;
		size_t Size = 0u;
		size_t Offset = 0u;
		uint64_t Version = 0u;
	};

	struct StagingBufferBlock
	{
		RHIBufferPtr Buffer;
		size_t Offset = 0u;
		uint32_t NumAlloc = 0u;
	};

	virtual size_t GetDefaultBlockSize() const { return 64u * Kilobyte; }
	virtual size_t GetDefaultAlignment() const { return 256u; }

	inline void OnStagingBufferAllocated(size_t UsedSize)
	{
		m_AllocatedMemorySize.fetch_add(GetDefaultBlockSize(), std::memory_order_relaxed);
		m_UsedMemorySize.fetch_add(UsedSize, std::memory_order_relaxed);
	}

	inline void OnStagingBufferFreed(size_t FreedSize)
	{
		m_UsedMemorySize.fetch_sub(FreedSize, std::memory_order_relaxed);
	}

	RHICommandBuffer* GetUploadCommandBuffer();
	StagingBuffer AcquireStagingBuffer(RHICommandBuffer* CommandBuffer, size_t Size, size_t Alignment);
	void QueueReleaseStagingBuffer(StagingBuffer& Buffer);
	void QueueSubmitUploadCommandBuffer(RHICommandBuffer* UploadCommandBuffer);

	RHIDevice& m_Device;

	std::mutex m_StagingBlockListLock;
	std::mutex m_PendingFreeLock;
	std::list<StagingBufferBlock> m_StagingBlockList;
	std::list<StagingBuffer> m_PendingFreeStagingBufferList;

	std::atomic<size_t> m_AllocatedMemorySize = 0u;
	std::atomic<size_t> m_UsedMemorySize = 0u;
};
