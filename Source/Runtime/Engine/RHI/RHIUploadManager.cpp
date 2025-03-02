#include "Engine/RHI/RHIUploadManager.h"
#include "Engine/RHI/RHICommandListContext.h"
#include "Engine/RHI/RHIBackend.h"

RHIUploadManager::StagingBuffer RHIUploadManager::AcquireStagingBuffer(RHICommandBuffer* CommandBuffer, size_t Size, size_t Alignment)
{
	assert(CommandBuffer && Size);

	thread_local StagingBufferBlock t_StagingBlock;

	if (t_StagingBlock.Buffer)
	{
		size_t AlignedOffset = Align(t_StagingBlock.Offset, Alignment);
		if (AlignedOffset <= t_StagingBlock.Buffer->GetSize())
		{
			t_StagingBlock.Offset = AlignedOffset + Size;
			t_StagingBlock.NumAlloc += 1u;

			return StagingBuffer{
				t_StagingBlock.Buffer.get(),
				CommandBuffer,
				&t_StagingBlock,
				Size,
				AlignedOffset,
				0u
			};
		}
	}

	size_t AlignedSize = Align(Size, Alignment);
	{
		std::lock_guard Locker(m_StagingBlockListLock);
		for (auto It = m_StagingBlockList.begin(); It != m_StagingBlockList.end(); ++It)
		{
			if (It->Buffer->GetSize() >= AlignedSize)
			{
				t_StagingBlock = std::move((*It));
				m_StagingBlockList.erase(It);

				t_StagingBlock.Offset = AlignedSize;
				t_StagingBlock.NumAlloc += 1;
				
				return StagingBuffer{
					t_StagingBlock.Buffer.get(),
					CommandBuffer,
					&t_StagingBlock,
					AlignedSize,
					AlignedSize,
					0u
				};
			}
		}

		if (t_StagingBlock.Buffer)
		{
			m_StagingBlockList.emplace_back(std::move(t_StagingBlock));
		}
	}

	RHIBufferCreateInfo CreateInfo;
	CreateInfo.SetSize(std::max(AlignedSize, GetDefaultBlockSize()))
		.SetAccessFlags(ERHIDeviceAccessFlags::GpuReadCpuWrite);
	t_StagingBlock.Buffer = m_Device.CreateBuffer(CreateInfo, nullptr);
	t_StagingBlock.Offset = AlignedSize;
	t_StagingBlock.NumAlloc += 1;

	return StagingBuffer{
		t_StagingBlock.Buffer.get(),
		CommandBuffer,
		&t_StagingBlock,
		AlignedSize,
		AlignedSize,
		0u
	};
}

void RHIUploadManager::QueueReleaseStagingBuffer(StagingBuffer& Buffer)
{
	std::lock_guard Locker(m_StagingBlockListLock);
	Buffer.Owner->PendingFreeStagingBufferList.emplace_back(std::move(Buffer));
}

void RHIUploadManager::QueueUploadBuffer(const RHIBuffer* Buffer, const void* Data, size_t Size, size_t SrcOffset)
{
}

void RHIUploadManager::QueueUploadTexture(const RHITexture* Texture, const void* Data, size_t Size, size_t SrcOffset)
{
}

void RHIUploadManager::QueueUploadTexture(const RHITexture* Texture, uint32_t ArrayLayer, uint32_t MipLevel, const void* Data, size_t Size, size_t SrcOffset)
{
}

RHICommandBuffer* RHIUploadManager::GetUploadCommandBuffer()
{
	RHICommandListContext* CommandListContext = nullptr;

	if (RHIBackend::GetConfigs().UseTransferQueue)
	{
		CommandListContext = m_Device.GetImmediateCommandListContext(ERHIDeviceQueue::Transfer);
	}
	else
	{
		thread_local std::shared_ptr<RHICommandListContext> t_CommandListContext;

		if (!t_CommandListContext)
		{
			t_CommandListContext = m_Device.AcquireDeferredCommandListContext();
		}

		CommandListContext = t_CommandListContext.get();
	}

	assert(CommandListContext);
	return CommandListContext->GetCommandBuffer();
}
