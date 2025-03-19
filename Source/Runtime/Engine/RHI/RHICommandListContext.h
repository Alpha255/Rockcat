#pragma once

#include "Engine/RHI/RHICommandBuffer.h"

class RHICommandListContext
{
public:
	virtual void SubmitGraphicsCommandBuffer() = 0;
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