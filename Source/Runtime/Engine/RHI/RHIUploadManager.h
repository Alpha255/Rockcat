#pragma once

#include "Engine/RHI/RHIBuffer.h"

class RHIUploadManager
{
public:
	RHIUploadManager(RHIDevice& Device)
		: m_Device(Device)
	{
	}

	inline size_t GetAllocatedMemorySize() const { return m_AllocatedMemorySize.load(); }
	inline size_t GetUsedMemorySize() const { return m_UsedMemorySize.load(); }

	void QueueUploadBuffer(const RHIBuffer* Buffer, const void* Data, size_t Size, size_t SrcOffset = 0u);
	void QueueUploadTexture(const RHITexture* Texture, const void* Data, size_t Size, size_t SrcOffset = 0u);
	void QueueUploadTexture(const RHITexture* Texture, uint32_t ArrayLayer, uint32_t MipLevel, const void* Data, size_t Size, size_t SrcOffset = 0u);
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

		std::list<StagingBuffer> PendingFreeStagingBufferList;
	};

	virtual size_t GetDefaultBlockSize() const { return 64u * Kilobyte; }
	virtual size_t GetDefaultAlignment() const { return 4u * Kilobyte; }

	inline void OnStagingBufferAllocated(const StagingBuffer& Buffer)
	{
		m_AllocatedMemorySize += GetDefaultBlockSize();
		m_UsedMemorySize += Buffer.Size;
	}

	inline void OnStagingBufferFreed(const StagingBuffer& Buffer)
	{
		m_UsedMemorySize -= Buffer.Size;
	}

	RHICommandBuffer* GetUploadCommandBuffer();
	StagingBuffer AcquireStagingBuffer(RHICommandBuffer* CommandBuffer, size_t Size, size_t Alignment);
	void QueueReleaseStagingBuffer(StagingBuffer& Buffer);

	RHIDevice& m_Device;

	std::mutex m_StagingBlockListLock;
	std::list<StagingBufferBlock> m_StagingBlockList;

	std::atomic<size_t> m_AllocatedMemorySize = 0u;
	std::atomic<size_t> m_UsedMemorySize = 0u;
};
