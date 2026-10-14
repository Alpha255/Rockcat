#pragma once

#include "Rendering/RHI/RHICommandBuffer.h"

class RHICommandListContext
{
public:
	/// Ends recording on the graphics command buffer, hands it to the backend for submission and
	/// moves it into the submitted state. Backends only implement SubmitCommandBuffer().
	void SubmitGraphicsCommandBuffer()
	{
		std::lock_guard Locker(m_PrimaryCommandBufferList.Lock);

		auto CommandBuffer = m_PrimaryCommandBufferList.Graphics;
		if (CommandBuffer && CommandBuffer->IsRecording())
		{
			CommandBuffer->End();
		}

		SubmitCommandBuffer(CommandBuffer);

		if (CommandBuffer)
		{
			CommandBuffer->SetStatus(RHICommandBuffer::EStatus::Submitted);

			/// Drop the cached handle: it no longer records anything. The next frame has to go
			/// through GetCommandBuffer() again so the buffer is recycled (reset + re-begun).
			/// Recording into a submitted command buffer is invalid.
			m_PrimaryCommandBufferList.Graphics = nullptr;
		}
	}

	virtual void SubmitUploadCommandBuffer(RHICommandBuffer* UploadCommandBuffer = nullptr) = 0;

	RHICommandBuffer* GetGraphicsCommandBuffer(ERHICommandBufferLevel Level = ERHICommandBufferLevel::Primary)
	{
		auto& CommandBufferList = Level == ERHICommandBufferLevel::Primary ? m_PrimaryCommandBufferList : m_SecondaryCommandBufferList;

		if (!CommandBufferList.Graphics)
		{
			CommandBufferList.Graphics = GetCommandBuffer(Level);
		}
		return CommandBufferList.Graphics;
	}

	RHICommandBuffer* GetUploadCommandBuffer(ERHICommandBufferLevel Level = ERHICommandBufferLevel::Primary)
	{
		auto& CommandBufferList = Level == ERHICommandBufferLevel::Primary ? m_PrimaryCommandBufferList : m_SecondaryCommandBufferList;

		if (!CommandBufferList.Upload)
		{
			CommandBufferList.Upload = GetCommandBuffer(Level);
		}
		return CommandBufferList.Upload;
	}

	RHICommandBuffer* GetUploadCommandBufferAsync(ERHICommandBufferLevel Level = ERHICommandBufferLevel::Primary)
	{
		auto& CommandBufferList = Level == ERHICommandBufferLevel::Primary ? m_PrimaryCommandBufferList : m_SecondaryCommandBufferList;

		std::lock_guard Locker(CommandBufferList.Lock);

		for (auto& CommandBuffer : CommandBufferList)
		{
			CommandBuffer->RefreshStatus();

			if (CommandBuffer->IsReady())
			{
				CommandBuffer->Begin();
				return CommandBuffer.get();
			}
			else if (CommandBuffer->IsNeedReset())
			{
				CommandBuffer->Reset();
				CommandBuffer->Begin();
				return CommandBuffer.get();
			}
		}

		auto CommandBuffer = CommandBufferList.emplace_back(AllocateCommandBuffer(Level)).get();
		assert(CommandBuffer);
		CommandBuffer->Begin();
		return CommandBuffer;
	}
protected:
	/// Backend hook used by SubmitGraphicsCommandBuffer().
	virtual void SubmitCommandBuffer(RHICommandBuffer* CommandBuffer) = 0;

	/// Ends recording and moves CommandBuffer into the submitted state. Backends call this from
	/// SubmitUploadCommandBuffer() right before handing the buffer to the queue; RHICommandBuffer
	/// only lets RHICommandListContext touch its status, hence the helper.
	void EndAndMarkSubmitted(RHICommandBuffer* CommandBuffer)
	{
		if (!CommandBuffer)
		{
			return;
		}

		if (CommandBuffer->IsRecording())
		{
			CommandBuffer->End();
		}

		CommandBuffer->SetStatus(RHICommandBuffer::EStatus::Submitted);
	}

	RHICommandBuffer* GetCommandBuffer(ERHICommandBufferLevel Level)
	{
		auto& CommandBufferList = Level == ERHICommandBufferLevel::Primary ? m_PrimaryCommandBufferList : m_SecondaryCommandBufferList;

		std::lock_guard Locker(CommandBufferList.Lock);

		for (auto& CommandBuffer : CommandBufferList)
		{
			CommandBuffer->RefreshStatus();

			if (CommandBuffer->IsRecording())
			{
				return CommandBuffer.get();
			}
			else if (CommandBuffer->IsReady())
			{
				CommandBuffer->Begin();
				return CommandBuffer.get();
			}
			else if (CommandBuffer->IsNeedReset())
			{
				CommandBuffer->Reset();
				CommandBuffer->Begin();
				return CommandBuffer.get();
			}
		}

		auto CommandBuffer = CommandBufferList.emplace_back(AllocateCommandBuffer(Level)).get();
		assert(CommandBuffer);
		CommandBuffer->Begin();
		return CommandBuffer;
	}

	virtual RHICommandBufferPtr AllocateCommandBuffer(ERHICommandBufferLevel Level) = 0;

	struct RHICommandBufferList : public std::vector<RHICommandBufferPtr>
	{
		std::mutex Lock;
		RHICommandBuffer* Graphics = nullptr;
		RHICommandBuffer* Upload = nullptr;
	};

	RHICommandBufferList m_PrimaryCommandBufferList;
	RHICommandBufferList m_SecondaryCommandBufferList;
};