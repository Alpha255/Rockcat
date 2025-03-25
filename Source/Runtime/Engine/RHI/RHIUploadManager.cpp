#include "Engine/RHI/RHIUploadManager.h"
#include "Engine/RHI/RHICommandListContext.h"
#include "Engine/RHI/RHIBackend.h"

thread_local std::shared_ptr<RHICommandListContext> t_CommandListContext;

RHIUploadManager::StagingBuffer RHIUploadManager::AcquireStagingBuffer(RHICommandBuffer* CommandBuffer, size_t Size, size_t Alignment)
{
	assert(CommandBuffer && Size);

	thread_local StagingBufferBlock t_StagingBlock;
	
	uint64_t Version = CommandBuffer->GetFenceSignaledCounter();

	if (t_StagingBlock.Buffer)
	{
		size_t AlignedOffset = Align(t_StagingBlock.Offset, Alignment);
		if (AlignedOffset <= t_StagingBlock.Buffer->GetSize())
		{
			t_StagingBlock.Offset = AlignedOffset + Size;
			std::atomic_ref<uint32_t>(t_StagingBlock.NumAlloc).fetch_add(1u);

			return StagingBuffer{
				t_StagingBlock.Buffer.get(),
				CommandBuffer,
				&t_StagingBlock,
				Size,
				AlignedOffset,
				Version
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
				std::atomic_ref<uint32_t>(t_StagingBlock.NumAlloc).fetch_add(1u);

				return StagingBuffer{
					t_StagingBlock.Buffer.get(),
					CommandBuffer,
					&t_StagingBlock,
					AlignedSize,
					AlignedSize,
					Version
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
	t_StagingBlock.Buffer = m_Device.CreateBuffer(CreateInfo);
	t_StagingBlock.Offset = AlignedSize;
	std::atomic_ref<uint32_t>(t_StagingBlock.NumAlloc).fetch_add(1u);

	OnStagingBufferAllocated(AlignedSize);

	return StagingBuffer{
		t_StagingBlock.Buffer.get(),
		CommandBuffer,
		&t_StagingBlock,
		AlignedSize,
		AlignedSize,
		Version
	};
}

void RHIUploadManager::QueueReleaseStagingBuffer(StagingBuffer& Buffer)
{
	std::lock_guard Locker(m_PendingFreeLock);
	m_PendingFreeStagingBufferList.emplace_back(std::move(Buffer));
}

void RHIUploadManager::QueueUploadBuffer(const RHIBuffer* Buffer, const void* Data, size_t Size, size_t SrcOffset)
{
	auto CommandBuffer = GetUploadCommandBuffer();
	auto StagingBuffer = AcquireStagingBuffer(CommandBuffer, Size, GetDefaultAlignment());

	auto Mapped = StagingBuffer.Buffer->Map(ERHIMapMode::WriteOnly, Size, StagingBuffer.Offset);
	VERIFY(memcpy_s(Mapped, Size, reinterpret_cast<const uint8_t*>(Data) + SrcOffset, Size) == 0);
	StagingBuffer.Buffer->Unmap();

	CommandBuffer->WriteBuffer(Buffer, StagingBuffer.Buffer, Size, StagingBuffer.Offset, 0u);

	QueueSubmitUploadCommandBuffer(CommandBuffer);
	QueueReleaseStagingBuffer(StagingBuffer);
}

void RHIUploadManager::QueueSubmitUploadCommandBuffer(RHICommandBuffer* UploadCommandBuffer)
{
	if (m_Device.SupportsTransferQueue())
	{
		auto CommandListContext = m_Device.GetImmediateCommandListContext(ERHIDeviceQueue::Transfer);
		CommandListContext->SubmitUploadCommandBuffer(UploadCommandBuffer);
	}
	else
	{
		assert(t_CommandListContext);
		t_CommandListContext->SubmitUploadCommandBuffer();
	}
}

void RHIUploadManager::QueueUploadTexture(const RHITexture* Texture, const void* Data, size_t Size, size_t SrcOffset)
{
	auto CommandBuffer = GetUploadCommandBuffer();
	auto StagingBuffer = AcquireStagingBuffer(CommandBuffer, Size, GetDefaultAlignment());

	auto Mapped = reinterpret_cast<uint8_t*>(StagingBuffer.Buffer->Map(ERHIMapMode::WriteOnly, Size, StagingBuffer.Offset));

	for (uint32_t ArrayLayer = 0u; ArrayLayer < Texture->GetNumArrayLayer(); ++ArrayLayer)
	{
		for (uint32_t MipLevel = 0u; MipLevel < Texture->GetNumMipLevel(); ++MipLevel)
		{
			uint32_t MipWidth = std::max(Texture->GetWidth() >> MipLevel, 1u);
			uint32_t MipHeight = std::max(Texture->GetHeight() >> MipLevel, 1u);
			uint32_t MipDepth = std::max(Texture->GetDepth() >> MipLevel, 1u);
			auto FormatAttributes = RHI::GetFormatAttributes(MipWidth, MipHeight, Texture->GetFormat());

			for (uint32_t Depth = 0u; Depth < MipDepth; ++Depth)
			{
				const uint8_t* Source = reinterpret_cast<const uint8_t*>(Data) + SrcOffset + FormatAttributes.SlicePitch * Depth;
				const size_t RowSize = FormatAttributes.RowPitch * FormatAttributes.NumRows;

				VERIFY(memcpy_s(Mapped, RowSize, Source, RowSize) == 0);

				Mapped += RowSize;
				Source += RowSize;
			}
		}
	}

	StagingBuffer.Buffer->Unmap();

	CommandBuffer->WriteTexture(Texture, StagingBuffer.Buffer, Size, StagingBuffer.Offset);

	QueueSubmitUploadCommandBuffer(CommandBuffer);
	QueueReleaseStagingBuffer(StagingBuffer);
}

void RHIUploadManager::QueueUploadTexture(const RHITexture* Texture, uint32_t ArrayLayer, uint32_t MipLevel, const void* Data, size_t Size, size_t SrcOffset)
{
	auto CommandBuffer = GetUploadCommandBuffer();
	auto StagingBuffer = AcquireStagingBuffer(CommandBuffer, Size, GetDefaultAlignment());

	auto Mapped = reinterpret_cast<uint8_t*>(StagingBuffer.Buffer->Map(ERHIMapMode::WriteOnly, Size, StagingBuffer.Offset));

	uint32_t MipWidth = std::max(Texture->GetWidth() >> MipLevel, 1u);
	uint32_t MipHeight = std::max(Texture->GetHeight() >> MipLevel, 1u);
	uint32_t MipDepth = std::max(Texture->GetDepth() >> MipLevel, 1u);
	auto FormatAttributes = RHI::GetFormatAttributes(MipWidth, MipHeight, Texture->GetFormat());

	for (uint32_t Depth = 0u; Depth < MipDepth; ++Depth)
	{
		const uint8_t* Source = reinterpret_cast<const uint8_t*>(Data) + SrcOffset + FormatAttributes.SlicePitch * Depth;

		for (uint32_t Row = 0u; Row < FormatAttributes.NumRows; ++Row)
		{
			VERIFY(memcpy_s(Mapped, FormatAttributes.RowPitch, Source, FormatAttributes.RowPitch) == 0);

			Mapped += FormatAttributes.RowPitch;
			Source += FormatAttributes.RowPitch;
		}
	}

	StagingBuffer.Buffer->Unmap();

	CommandBuffer->WriteTexture(Texture, StagingBuffer.Buffer, Size, ArrayLayer, MipLevel, StagingBuffer.Offset);

	QueueSubmitUploadCommandBuffer(CommandBuffer);
	QueueReleaseStagingBuffer(StagingBuffer);
}

RHICommandBuffer* RHIUploadManager::GetUploadCommandBuffer()
{
	RHICommandListContext* CommandListContext = nullptr;

	if (m_Device.SupportsTransferQueue())
	{
		CommandListContext = m_Device.GetImmediateCommandListContext(ERHIDeviceQueue::Transfer);
		assert(CommandListContext);
		return CommandListContext->GetUploadCommandBufferAsync();
	}
	else
	{
		if (!t_CommandListContext)
		{
			t_CommandListContext = m_Device.AcquireDeferredCommandListContext();
		}

		CommandListContext = t_CommandListContext.get();
		assert(CommandListContext);
		return CommandListContext->GetUploadCommandBuffer();
	}
}

void RHIUploadManager::FlushPendingFreeStagingBuffers()
{
	for (auto It = m_PendingFreeStagingBufferList.begin(); It != m_PendingFreeStagingBufferList.end(); ++It)
	{
		if (It->CommandBuffer->GetFenceSignaledCounter() > It->Version)
		{
			if (std::atomic_ref<uint32_t>(It->Owner->NumAlloc).fetch_sub(1u) == 0u)
			{
				{
					std::lock_guard Locker(m_StagingBlockListLock);
					m_StagingBlockList.emplace_back(std::move(*(It->Owner)));
				}

				{
					std::lock_guard Locker(m_PendingFreeLock);
					It = m_PendingFreeStagingBufferList.erase(It);
				}
			}
		}
	}
}
