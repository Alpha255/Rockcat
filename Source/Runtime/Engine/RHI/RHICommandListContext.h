#pragma once

#include "Engine/RHI/RHICommandBuffer.h"

class RHICommandListContext
{
public:
	RHICommandBuffer* GetCommandBuffer(ERHICommandBufferLevel Level = ERHICommandBufferLevel::Primary)
	{
		auto& CommandBufferList = Level == ERHICommandBufferLevel::Primary ? m_PrimaryCommandBufferList : m_SecondaryCommandBufferList;
		std::lock_guard Locker(CommandBufferList.Lock);

		if (CommandBufferList.Active && CommandBufferList.Active->IsRecording())
		{
			return CommandBufferList.Active;
		}

		for (auto& CommandBuffer : CommandBufferList)
		{
			CommandBuffer->RefreshStatus();
			RHICommandBuffer*& ActiveCommandBuffer = CommandBufferList.Active;

			if (CommandBuffer->IsRecording())
			{
				ActiveCommandBuffer = CommandBuffer.get();
			}
			else if (CommandBuffer->IsReady())
			{
				CommandBuffer->Begin();
				ActiveCommandBuffer = CommandBuffer.get();
			}
			else if (CommandBuffer->IsNeedReset())
			{
				CommandBuffer->Reset();
				CommandBuffer->Begin();
				ActiveCommandBuffer = CommandBuffer.get();
			}

			if (ActiveCommandBuffer)
			{
				return ActiveCommandBuffer;
			}
		}

		CommandBufferList.Active = CommandBufferList.emplace_back(AllocateCommandBuffer(Level)).get();
		assert(CommandBufferList.Active);
		CommandBufferList.Active->Begin();
		return CommandBufferList.Active;
	}

	virtual void SubmitActiveCommandBuffer() = 0;
protected:
	virtual RHICommandBufferPtr AllocateCommandBuffer(ERHICommandBufferLevel Level) = 0;

	struct RHICommandBufferList : public std::vector<RHICommandBufferPtr>
	{
		std::mutex Lock;
		RHICommandBuffer* Active = nullptr;
	};

	RHICommandBufferList m_PrimaryCommandBufferList;
	RHICommandBufferList m_SecondaryCommandBufferList;
};